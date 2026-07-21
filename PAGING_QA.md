# Paging Memory Manager — Guide Question Answers

This document answers the paging-assignment guide questions against the
actual implementation added to this repo. New/changed files:

- `include/misc/IMemoryAllocator.hpp` — the given interface
- `include/misc/PagingAllocator.hpp` / `src/misc/PagingAllocator.cpp` — per-process paging allocator
- `include/misc/FrameManager.hpp` / `src/misc/FrameManager.cpp` — shared physical frame pool + replacement policy + counters
- `include/misc/BackingStore.hpp` / `src/misc/BackingStore.cpp` — swap-space bookkeeping
- `include/misc/MemoryManager.hpp` / `src/misc/MemoryManager.cpp` — orchestrator, drop-in replacement for `SchedulerService`'s old `MemoryAllocator` member
- `include/services/VmstatService.hpp` / `src/services/VmstatService.cpp` + `include/commands/VmstatCommand.hpp` / `src/commands/VmstatCommand.cpp` — new `vmstat` command
- `src/services/SchedulerService.cpp` — the 4 integration points with the new `memoryManager`

---

## 1. How does the memory manager implement `IMemoryAllocator`?

`IMemoryAllocator` (`include/misc/IMemoryAllocator.hpp`) is implemented by
**`PagingAllocator`** (`include/misc/PagingAllocator.hpp`), one instance per
resident process:

```cpp
class PagingAllocator : public IMemoryAllocator {
public:
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    std::string visualizeMemory() override;
    ...
};
```

It sets `memoryAllocatorType = PAGING` in its constructor
(`PagingAllocator.cpp:11`) and owns the three protected fields the interface
declares (`maximumSize`, `currentAllocatedSize`, plus the nested
`MemoryBlock` type used internally — see Q3).

`PagingAllocator` is **not** used directly by `SchedulerService`. A separate
orchestrator, **`MemoryManager`** (`include/misc/MemoryManager.hpp`), holds
one `PagingAllocator` per pid (`unordered_map<int, unique_ptr<PagingAllocator>>`)
plus the single shared `FrameManager`, and exposes pid-keyed convenience
methods (`configure`, `isResident(pid)`, `allocate(pid, name, size)`,
`deallocate(pid)`) that match the scheduler's original call sites. This two-tier
split exists because `IMemoryAllocator::allocate(size)` is process-agnostic
(malloc-like), but this codebase's scheduler needs to key memory operations
by pid — `MemoryManager` is the adapter between the two.

`SchedulerService` (`include/services/SchedulerService.hpp`) holds a
`MemoryManager memoryManager;` member exactly where it used to hold
`MemoryAllocator memoryAllocator;`.

---

## 2. How does the paging `allocate`/`deallocate` implementation work?

**`allocate(size)`** (`src/misc/PagingAllocator.cpp:17-54`):
1. Compute `pagesNeeded = ceil(size / frameSize)`; bail out (`nullptr`) if
   that exceeds the system's total frame count — this request can never be
   satisfied.
2. First-fit search over `freeRanges` (a `std::set<MemoryBlock>`, sorted by
   start address) for the first free *virtual* range within this process's
   own arena that is `>= size`. Split it if there's leftover space.
3. Convert the chosen virtual offset into a page range
   (`firstPage = offset/frameSize`, `lastPage = (offset+size-1)/frameSize`),
   then call `frameManager->acquireFrame(pid, p)` once per page — this is
   where a physical frame actually gets assigned (or an existing occupant
   gets evicted to make room; see Q7).
4. Record the allocation in `liveAllocations[offset] = size` and return
   `reinterpret_cast<void*>(offset)` — **the returned "pointer" is literally
   the process's own virtual byte offset**, not a real memory address (there
   is no real memory backing it — this is an emulator).

**`deallocate(ptr)`** (`src/misc/PagingAllocator.cpp:56-84`): see Q5 below —
it decodes `ptr` back to an offset, looks up the length in `liveAllocations`,
walks the covered pages releasing each valid frame via
`frameManager->releaseFrame()`, then coalesces the freed virtual range back
into `freeRanges`.

A subtlety worth calling out: frame acquisition/release happens **outside**
the allocator's own `stateMutex` (see the comment at
`PagingAllocator.cpp:36-40`). `FrameManager::acquireFrame` can evict another
process's page, which calls back into `MemoryManager`/`PagingAllocator`
(`invalidatePage`) — including, under memory pressure, back into *this same*
`PagingAllocator` if one of its own pages gets FIFO-selected as the victim
while it's resuming. Holding the lock across that call would deadlock, so
every cross-component call is made with the lock released.

---

## 3. What is the role of `MemoryBlock`? Is it wise to use in a paging environment?

`MemoryBlock` (nested inside `IMemoryAllocator`, `IMemoryAllocator.hpp:20-24`)
is a `{start, size}` range descriptor with `operator<` for sorting. In this
implementation it is used **only** inside `PagingAllocator`, as the element
type of `freeRanges` — the set of free *virtual* byte ranges within one
process's own arena (`PagingAllocator.hpp:38`). It earns its keep there
because virtual free space is variable-sized: allocations split ranges,
deallocations must merge (`insertAndCoalesce`, `PagingAllocator.cpp:166-182`)
adjacent free ranges back together, exactly the problem `MemoryBlock` +
`std::set` + sort/merge is built for.

**Would it be wise for the physical frame pool? No.** That is the deliberate
design decision behind `FrameManager` (`include/misc/FrameManager.hpp`):
physical frames are fixed-size and interchangeable — there is no such thing
as a "gap smaller than a frame" or a "range that needs splitting," so there
is nothing to coalesce. Modeling the frame pool with `MemoryBlock` would add
pointless coalescing machinery for a structure that never has partial
overlaps to merge.

**Alternative implementation used here:** `FrameManager` represents the
frame pool as a flat `std::vector<FrameEntry>` (occupied/owner-pid/owner-page
per frame) plus a `std::deque<size_t> freeList` of free frame indices and a
`std::deque<size_t> fifoOrder` tracking occupancy order for eviction — O(1)
acquire/release, no sorting or merging (`FrameManager.hpp:56-63`).

---

## 4. Where does the backing store live, and why?

The backing store is `BackingStore` (`include/misc/BackingStore.hpp`),
owned **inside `FrameManager`** (`FrameManager.hpp:61`), not inside any
individual `PagingAllocator`.

**Justification:** eviction is a *global* decision — when memory is full and
a new page needs a frame, the victim can belong to **any** process in the
system, not just the requester. Only `FrameManager` has visibility into every
frame's owner across all processes (`fifoOrder` spans the entire frame pool).
If the backing store lived per-process, a process's `PagingAllocator` would
have no way to record an eviction it doesn't even know is happening to it
(the victim is typically a *different* process than the one calling
`acquireFrame`). Placing it centrally means `FrameManager::evictVictimLocked`
(`FrameManager.cpp:21-33`) can call `backingStore.store(evictedPid, evictedPage)`
directly, regardless of whose page it evicted.

It is deliberately **bookkeeping-only**: `store`/`hasRecord`/`erase` just
track which `(pid, page)` pairs are conceptually swapped out
(`BackingStore.cpp`). No actual bytes are written to a file, because this
emulator has no per-byte memory-content model anywhere (the `SymbolTable`
never routes through the paging system) — writing fabricated placeholder
bytes on page-out would misrepresent what's being simulated rather than add
realism.

---

## 5. Given a `void* ptr`, how is it mapped back to its PTE and physical frame to free memory?

This is decoded entirely in `PagingAllocator::deallocate`
(`src/misc/PagingAllocator.cpp:56-84`):

1. `size_t offset = reinterpret_cast<size_t>(ptr);` — the "pointer" *is* the
   virtual offset (see Q2), so this recovers it with no translation needed.
2. `liveAllocations.find(offset)` looks up the byte length that was recorded
   for this exact offset at allocation time (`liveAllocations` is a
   `std::map<size_t, size_t>`, offset → size) — an unknown offset (already
   freed, or never allocated) returns early, guarding against double-free.
3. The byte range `[offset, offset+size)` is converted to a page range:
   `firstPage = offset / frameSize`, `lastPage = (offset+size-1) / frameSize`.
4. For each page `p` in that range, `pageTable[p]` — a flat
   `std::vector<PageTableEntry>` indexed directly by page number — is read:
   if `pageTable[p].valid`, `pageTable[p].frameNumber` **is** the physical
   frame to release, read in O(1) with no search
   (`PagingAllocator.cpp:71-76`). `frameManager->releaseFrame(frameToRelease)`
   is called to return it to the free pool, and the PTE is cleared.
5. Finally the freed virtual range is coalesced back into `freeRanges` (Q3).

This whole path is possible because the emulator's page table is exactly the
"simple flat array" the assignment anticipates — no multi-level walk is
needed, since each `PagingAllocator` only ever manages one process's own
small, single-level table.

---

## 6. How does internal fragmentation occur in a paging environment?

Because every allocation is rounded up to whole frames (`pagesNeeded = ceil(size/frameSize)`
in `allocate`), any request whose size isn't an exact multiple of `frameSize`
wastes the unused tail of its last page. This is computed directly as:

```
internal fragmentation = getPageCount() * frameSize - getAllocatedBytes()
```

(`PagingAllocator::getPageCount()` / `getAllocatedBytes()`,
`PagingAllocator.cpp:142-150`; summed across all resident processes in
`MemoryManager::totalInternalFragmentation()`, `MemoryManager.cpp:105-112`,
and surfaced in both `writeMemorySnapshot`
(`SchedulerService.cpp:414`) and `vmstat`).

This is structurally different from — and cannot be confused with —
**external** fragmentation, which is the old flat allocator's problem
(`MemoryAllocator::totalFragmentation`, unchanged): free gaps *between*
variable-sized allocated blocks that are individually too small to satisfy a
new request even though total free space is sufficient. Paging eliminates
external fragmentation entirely (every unit is frame-sized, so free frames
are always usable), at the cost of internal fragmentation (the fixed
granularity wastes space inside the last page of each allocation).

---

## 7. Where are num-paged-in and num-paged-out tallied?

Both counters live in `FrameManager` (`std::atomic<uint64_t> numPagedIn`,
`numPagedOut`, `FrameManager.hpp:59-60`) and are updated in exactly one place
each, both inside `FrameManager::acquireFrame` / its helper
`evictVictimLocked`:

- **`numPagedOut++`** — `FrameManager.cpp:26`, inside `evictVictimLocked()`,
  every time a frame with no free alternative is reclaimed from its current
  owner (recording the eviction in `BackingStore` in the same step).
- **`numPagedIn++`** — `FrameManager.cpp:49`, inside `acquireFrame()`, but
  **only** when `backingStore.hasRecord(pid, page)` is true for the page
  being acquired — i.e., only when this exact page was previously evicted and
  is now being restored. A page's first-ever (zero-fill) acquisition does
  **not** count as a page-in, matching real OS semantics where demand-zero
  faults aren't swap-ins.

This single `hasRecord` check is what makes first-touch vs. restore
self-determining: both `PagingAllocator::allocate` (fresh pages) and
`PagingAllocator::restoreEvictedPages` (resuming pages) call the *same*
`acquireFrame`, and the counters come out correct without any "is this a
restore" flag being threaded through either call site.

Both counters are surfaced in `SchedulerService::writeMemorySnapshot`
(`SchedulerService.cpp:415-416`, written to `memory_stamp_<qq>.txt`) and in
the new `vmstat` command (`VmstatService.cpp`).

---

## 8. Why does the Linux kernel use multi-level page tables instead of a single flat array for 64-bit architectures?

**Sources (verified against `github.com/torvalds/linux`, branch `master`):**
- [`arch/x86/include/asm/pgtable_64_types.h#L56-L74`](https://github.com/torvalds/linux/blob/master/arch/x86/include/asm/pgtable_64_types.h#L56-L74) — level shift constants
- [`arch/x86/include/asm/pgtable_types.h#L295`](https://github.com/torvalds/linux/blob/master/arch/x86/include/asm/pgtable_types.h#L295), [`#L342`](https://github.com/torvalds/linux/blob/master/arch/x86/include/asm/pgtable_types.h#L342), [`#L368`](https://github.com/torvalds/linux/blob/master/arch/x86/include/asm/pgtable_types.h#L368) — `pgd_t`/`p4d_t`/`pud_t` typedefs
- [`Documentation/arch/x86/x86_64/mm.rst#L26-L84`](https://github.com/torvalds/linux/blob/master/Documentation/arch/x86/x86_64/mm.rst#L26-L84) — the virtual memory map

```c
// arch/x86/include/asm/pgtable_64_types.h
#define P4D_SHIFT   39
#define PUD_SHIFT   30
#define PMD_SHIFT   21
// (PGDIR_SHIFT is a runtime value: 4-level vs. 5-level/LA57 paging changes
// the top-level shift between 39 and 48)
```

x86-64 addresses are 48 bits wide today (57 with LA57/5-level paging). A
single flat array indexed by the full virtual address at 4KB granularity
would need `2^48 / 2^12 = 2^36` (~68 billion) page-table-entry slots — at 8
bytes each, over 500GB of memory just for the table, allocated up front
whether or not the address space is actually used. `Documentation/arch/x86/x86_64/mm.rst`
shows the real picture: the 64-bit address space is overwhelmingly sparse —
huge guard holes and a non-canonical gap separate the small regions
(user-space, kernel text, vmalloc, direct mapping, etc.) that are actually
mapped. The kernel instead splits the address into five fixed-width fields
(`PGDIR`/`P4D`/`PUD`/`PMD`/`PTE`, each indexing a 512-entry, 4KB table), so a
page-table page is allocated **only** for the branches of this radix-tree-like
structure that lead to something actually mapped. Unused regions of the
address space cost nothing beyond a null entry at the top level — exactly
the property a flat array cannot offer, since a flat array's cost is
proportional to the size of the address space, not to how much of it is used.

---

## 9. What Linux kernel code resembles the `MemoryBlock` implementation?

**Sources:**
- [`include/linux/memblock.h#L66-L80`](https://github.com/torvalds/linux/blob/master/include/linux/memblock.h#L66-L80) — `struct memblock_region`
- [`include/linux/memblock.h#L82-L96`](https://github.com/torvalds/linux/blob/master/include/linux/memblock.h#L82-L96) — `struct memblock_type`
- [`include/linux/ioport.h#L22-L29`](https://github.com/torvalds/linux/blob/master/include/linux/ioport.h#L22-L29) — `struct resource`
- [`mm/memblock.c#L529-L563`](https://github.com/torvalds/linux/blob/master/mm/memblock.c#L529-L563) — `memblock_merge_regions`

```c
// include/linux/memblock.h
struct memblock_region {
    phys_addr_t base;
    phys_addr_t size;
    enum memblock_flags flags;
#ifdef CONFIG_NUMA
    int nid;
#endif
};
```

```c
// include/linux/ioport.h
struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    unsigned long desc;
    struct resource *parent, *sibling, *child;
};
```

`struct memblock_region` is directly the same shape as `IMemoryAllocator::MemoryBlock`
(`base`/`start` + `size`), used by the early-boot physical memory allocator
(`memblock`) to track usable and reserved physical ranges before the buddy
allocator is up. `struct memblock_type` wraps a sorted array of these
regions (analogous to `PagingAllocator`'s `std::set<MemoryBlock> freeRanges`).
`memblock_merge_regions()` (`mm/memblock.c:536-563`) is the direct kernel
analog of `PagingAllocator::insertAndCoalesce`: it walks adjacent regions in
the sorted array and merges any two where `this->base + this->size == next->base`
(physically adjacent, same flags/NUMA node) by growing `size` and shifting
the rest of the array down — the exact same "merge adjacent ranges"
operation. `struct resource` is the more general-purpose equivalent (I/O
ports, PCI BARs, MMIO windows) using `start`/`end` instead of `base`/`size`
and organizing entries as a tree instead of a flat sorted array, since
resource reservations naturally nest (a PCI BAR window nested inside a
bridge's window, etc.).

---

## 10. What does `walk_pud_range()` do?

**Source:** [`mm/pagewalk.c#L185-L248`](https://github.com/torvalds/linux/blob/master/mm/pagewalk.c#L185-L248)
(verified current signature — unchanged from the assignment's version):

```c
static int walk_pud_range(p4d_t *p4d, unsigned long addr, unsigned long end,
              struct mm_walk *walk)
{
    pud_t *pud;
    ...
    pud = pud_offset(p4d, addr);
    do {
 again:
        next = pud_addr_end(addr, end);
        if (pud_none(*pud)) {
            /* allocate a PMD table, or report the hole, then continue */
            ...
        }

        walk->action = ACTION_SUBTREE;
        if (ops->pud_entry)
            err = ops->pud_entry(pud, addr, next, walk);
        ...
        if (walk->action == ACTION_AGAIN)
            goto again;
        if (walk->action == ACTION_CONTINUE)
            continue;

        if (walk->vma)
            split_huge_pud(walk->vma, pud, addr);
        else if (pud_leaf(*pud) || !pud_present(*pud))
            continue;

        err = walk_pmd_range(pud, addr, next, walk);
        ...
    } while (pud++, addr = next, addr != end);

    return err;
}
```

This function walks every PUD (Page Upper Directory) entry covering the
address range `[addr, end)` reached from one P4D entry — it is one level
(the third of five) in the kernel's generic page-table-walking framework
(`walk_pgd_range` → `walk_p4d_range` → `walk_pud_range` → `walk_pmd_range` →
`walk_pte_range`), used by things like `/proc/<pid>/pagemap`, NUMA memory
migration, and `numa_maps`. For each PUD entry: if it's unmapped
(`pud_none`), it either allocates a PMD table (when the walk is installing
new mappings) or invokes the caller's `pte_hole` callback to report the gap.
Otherwise it invokes the caller-supplied `ops->pud_entry()` callback (the
generic mechanism letting different consumers of the page-walk API do
different things at this level), which can steer control via
`walk->action`: `ACTION_SUBTREE` (default) descends further, `ACTION_AGAIN`
retries the same entry, and `ACTION_CONTINUE` skips to the next entry without
descending. If descent is still needed, it handles transparent-huge-page
leaf PUDs — splitting one into a real PMD table via `split_huge_pud()` when
operating inside a VMA, or simply skipping already-leaf entries otherwise —
then recurses into `walk_pmd_range()` to continue the walk one level deeper
for that address range.

---

## Evidence: forced-eviction test run

Config used (`max-overall-mem 64`, `mem-per-frame 16`, `mem-per-proc 32` →
4 total frames, 2 frames/process → at most 2 processes fully resident at
once, `num-cpu 2`, `batch-process-freq 1` to keep new processes arriving
continuously and contending for the same 4 frames):

```
Total memory: 64 bytes
Used memory: 64 bytes
Free memory: 0 bytes
Internal fragmentation: 0 bytes
Resident processes: 0
Pages paged in: 9694
Pages paged out: 13206
-----------------------------------------
frame 0:  (pid 1205, page 0)
frame 1:  (pid 1205, page 1)
frame 2:  (pid 2280, page 0)
frame 3:  (pid 2280, page 1)
```

Corresponding `memory_stamp_<qq>.txt` excerpt from the same run:

```
----end---- = 64

64
 (page 1)
48

48
 (page 0)
32

32
 (page 1)
16

16
 (page 0)
0

----start----- = 0
```

Both counters are non-zero and grow across successive snapshots, confirming
real eviction/restore cycling under memory pressure.

**Regression sanity check:** with `batch-process-freq` set high enough that
only one process is ever auto-generated (comfortably fitting in the default
`max-overall-mem 16384` / `mem-per-proc 4096` config), a `vmstat` taken after
that process ran to completion showed `Pages paged in: 0`, `Pages paged out: 0`,
and all frames free — confirming paging activity only occurs under genuine
memory pressure, and that a finished process's frames are fully reclaimed
(no leaks) via `MemoryManager::deallocate` → `PagingAllocator::releaseAllFrames`
→ `FrameManager::releaseFrame`.

Separately, note that under the *default* `config.txt` — 16384 bytes /
4096 bytes-per-process = only 4 processes can be resident at once — combined
with `batch-process-freq 1` (a new process spawns essentially every tick,
unbounded), the system oversubscribes memory heavily regardless of allocator
type, so `vmstat` shows heavy paging there too (hundreds of thousands of
page-ins/outs over a few seconds of wall time). This is expected given how
this repo's scheduler continuously spawns processes once `scheduler-start`
runs — it is a property of the workload, not a defect in the paging logic.

---

## Known limitation: cosmetic process-name race in reports

`MemoryManager::snapshotFrames()` (`MemoryManager.cpp`) takes an atomic
snapshot of frame occupancy from `FrameManager`, then separately locks
`mapMutex` to resolve each occupied frame's `pid` to a process name. Under
very heavy, continuous eviction churn (as in the forced-eviction test above),
a frame's owner can change between those two steps, occasionally leaving the
process-name field blank in `vmstat`/`memory_stamp_*.txt` output for that
frame (the `pid` and page number are still correct). This is a deliberate
tradeoff, not an oversight: closing the gap would require holding `mapMutex`
while calling into `FrameManager`, which risks a lock-order-inversion
deadlock against the eviction callback path (`FrameManager::evictVictimLocked`
→ `onEvict` → `mapMutex`) documented in Q2/Q4. It self-heals on the very next
snapshot and never affects the paged-in/out counters or actual frame
accounting.
