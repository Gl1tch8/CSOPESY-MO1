# CSOPESY MO1 — OS Emulator / Process Scheduler
## Presentation Notes & Code Walkthrough

A multi-threaded command-line OS emulator in C++. The user drives it through a
REPL; behind the prompt, a configurable scheduler runs a fleet of CPU-core
threads that execute randomly-generated "programs" instruction-by-instruction.

Architecture at a glance — a layered **Command → Service** design:

```
main() REPL  ─►  Helper::parse  ─►  *Command (thin)  ─►  *Service (logic)
                                                              │
                          SchedulerService ──► CPU core threads ──► InstructionParser
                                                              │
                                                        SystemState (singleton)
```

---

## 1. Command Recognition

**Idea.** Every line typed at the `root:\>` prompt is reduced to a single
*keyword* — the first whitespace-delimited token. That keyword is what the REPL
matches on. Recognition is deliberately split from interpretation: `Helper::parse`
only *identifies* the command; the dispatch loop decides what to do with it.

**Tokenizer** — [src/misc/Helper.cpp:26](src/misc/Helper.cpp#L26)

```cpp
std::string Helper::parse(std::string input) {
    // parse first word only and return that
    std::string command = input.substr(0, input.find(" "));
    return command;
}
```

**Recognition rules** — [src/main.cpp:46-86](src/main.cpp#L46-L86)

The REPL enforces a *gate*: until `initialize` has run, only `initialize` and
`exit` are recognized. Everything else is rejected. This guarantees the system
is configured before any process work begins.

```cpp
command = helper.parse(input);

if (!SystemState::getInstance().isInitialized()) {
    if (command == "initialize") {
        initializeCommand->execute(input);
        state.start();                      // begin ticking after init
        schedulerService->initScheduler();  // bring CPU cores online
    }
    else if (command == "exit") { exitCommand->execute(input); isRunning = false; }
    else {
        std::cout << "Error: 'initialize' must be called before any "
                     "other command is recognized." << std::endl;
        continue;
    }
}
else if (command == "screen")          { screenCommand->execute(input); }
else if (command == "scheduler-start") { schedulerCommand->executeStart(input); }
else if (command == "scheduler-stop")  { schedulerCommand->executeStop(input); }
else if (command == "report-util")     { reportUtilCommand->execute(input); }
else if (command == "exit")            { state.stop(); exitCommand->execute(input); ... }
```

**Recognized keywords:** `initialize`, `screen`, `scheduler-start`,
`scheduler-stop`, `report-util`, `clear`, `exit`.

**Talking points**
- One-token recognition keeps the grammar simple; arguments/flags (`-s`, `-r`,
  process names) are re-parsed *inside* each command, not at recognition time.
- The initialization gate is a state-machine guard, not just a check — it makes
  illegal command sequences impossible.

---

## 2. Console UI Implementation

**Idea.** A text UI (TUI) over `std::cin`/`std::cout`. The main menu prints an
ASCII banner; the REPL renders the `root:\>` prompt; nested screen sessions get
their own prompt loop with scroll-back history.

**Main menu / banner** — [src/tui/MainMenu.cpp:4-32](src/tui/MainMenu.cpp#L4-L32)

```cpp
void MainMenuTUI::printMainMenu() {
    std::string asciiArt = R"(
 _______  _______  _______  _______  _______  _______
(  ____ \(  ____ \(  ___  )(  ____ )(  ____ \(  ____ \|\     /|
 ... CSOPESY banner ... )";

    std::string headerText = asciiArt +
        "\nHello, Welcome to CSOPESY commandline!"
        "\nType 'exit' to quit, 'clear' to clear the screen";
    headerText += "\n** IMPORTANT: Type 'initialize' to load config and start system **\n";
    std::cout << headerText << std::endl;
}

void MainMenuTUI::clear() {            // cross-platform screen clear
    #if defined(_WIN32)
        system("cls");
    #elif defined(__linux__) || defined(__APPLE__)
        system("clear");
    #endif
}
```

**Main REPL loop** — [src/main.cpp:38-45](src/main.cpp#L38-L45)

```cpp
MainMenuTUI::printMainMenu();
while (isRunning) {
    std::cout << "root:\\>";
    std::getline(std::cin, input);
    if (!std::cin) {              // EOF / closed stdin → stop cleanly
        state.stop(); isRunning = false; break;
    }
    ...
}
```

**Nested "screen" session UI** — [src/commands/ScreenCommand.cpp:35-57](src/commands/ScreenCommand.cpp#L35-L57)

A screen session is a *sub-shell*: it clears the display, replays the saved
session log (`screen -r`), then runs its own input loop with `process-smi` /
`exit` commands. Every line is mirrored into a per-session log for restore.

```cpp
auto logAndPrint = [&](const std::string& text) {
    std::cout << text << std::endl;
    screenService->addSessionLog(activeScreenName, text + "\n");
};

while (screenService->hasActiveScreen()) {
    std::cout << "root:\\> ";
    std::getline(std::cin, input);
    if (!std::cin) { screenService->clearActiveScreen(); break; }
    screenService->addSessionLog(activeScreenName, "root:\\> " + input + "\n");

    if (input == "process-smi")
        logAndPrint(screenMuxService->processSMI(activeScreenName));
    else if (input == "exit") { screenService->clearActiveScreen(); ... }
    else logAndPrint("Unknown command. Use 'process-smi' or 'exit'.");
}
```

**`report-util` rendered view** — [src/services/ReportUtilService.cpp:11-23](src/services/ReportUtilService.cpp#L11-L23)
formats CPU utilization, cores used/available, and running/finished process
tables — printed to console *and* written to `csopesy-log.txt`.

**Talking points**
- Pure standard-library TUI — portable, no curses dependency.
- Two loop levels: the top REPL and the per-screen sub-shell, each with its own
  prompt and command set. (Note: the repo also ships an optional ImGui GUI under
  `src/gui/`, but the graded console flow is the TUI shown here.)

---

## 3. Command Interpreter Implementation

There are **two** interpreter layers — don't conflate them:

### 3a. Command-level interpreter (Command/Service pattern)

**Idea.** Each recognized keyword maps to a `Command` object that is a thin
adapter; the real work lives in a `Service`. The abstract base fixes the
contract.

**Abstract `Command`** — [src/interfaces/Command.hpp:10-24](src/interfaces/Command.hpp#L10-L24)

```cpp
class Command {
public:
    Service& service;                       // each command owns its service
    Command(Service& service) : service(service) {}
    virtual void print(std::string log) = 0;     // render result to console
    virtual void execute(std::string input) = 0; // run the command
};
```

**Concrete command** (ScreenCommand) — re-parses flags, then delegates —
[src/commands/ScreenCommand.cpp:12-26](src/commands/ScreenCommand.cpp#L12-L26)

```cpp
void ScreenCommand::execute(std::string input) {
    std::string log = this->service.executeFlags(input);  // -ls etc. handled here
    if (!log.empty()) { this->print(log); return; }

    std::stringstream ss(input);
    std::string cmd, flag;
    ss >> cmd >> flag;                       // re-tokenize for -s / -r
    ...
}
```

The Scheduler command shows the same delegation — it just forwards the raw
input to the service, which interprets the flag — [src/commands/SchedulerCommand.cpp:11-21](src/commands/SchedulerCommand.cpp#L11-L21):

```cpp
void SchedulerCommand::executeStart(std::string input) {
    std::string log = this->service.executeFlags(input);
    this->print(log);
}
```

[src/services/SchedulerService.cpp:14-26](src/services/SchedulerService.cpp#L14-L26) interprets the flag string:

```cpp
std::string SchedulerService::executeFlags(std::string input) {
    if (input.find("scheduler-start") != std::string::npos) { start(); return "Scheduler started."; }
    if (input.find("scheduler-stop")  != std::string::npos) { stop();  return "Scheduler stopped."; }
    return "Error: unrecognized scheduler command.";
}
```

### 3b. Instruction-level interpreter (the mini-language)

**Idea.** Each process is a small program of `Instruction`s. The
`InstructionParser` is a tree-walking interpreter over a vector of instructions,
with `FOR` carrying a nested body (so it recurses).

**Instruction model & opcodes** — [include/misc/SymbolTable.hpp:15-31](include/misc/SymbolTable.hpp#L15-L31)

```cpp
enum class OperationCode { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

struct Instruction {
    OperationCode opCode;
    std::vector<std::string> operands;
    std::vector<Instruction> body;   // nested instructions for FOR
    uint8_t repeats = 0;             // FOR iteration count
};
```

**The interpreter loop** — [src/misc/SymbolTable.cpp:31-72](src/misc/SymbolTable.cpp#L31-L72)

```cpp
void InstructionParser::executeBlock(const std::vector<Instruction>& instructions,
                                     uint64_t& tick) {
    for (const auto& instr : instructions) {
        if (!running) return;                 // cooperative cancel
        switch (instr.opCode) {
        case OperationCode::DECLARE: declare(instr.operands[0], instr.operands[1]); tick++; break;
        case OperationCode::PRINT:   print(instr.operands[0]); tick++; break;
        case OperationCode::ADD: {
            uint16_t r = add(resolveValue(instr.operands[1]), resolveValue(instr.operands[2]));
            symbolTable.setSymbol(instr.operands[0], r); tick++; break;
        }
        case OperationCode::SUBTRACT: { /* mirror of ADD, clamps at 0 */ tick++; break; }
        case OperationCode::SLEEP:
            sleep(static_cast<uint8_t>(std::stoul(instr.operands[0])));
            tick += static_cast<uint8_t>(std::stoul(instr.operands[0])); break;
        case OperationCode::FOR:
            forLoop(instr.body, instr.repeats); tick++; break;   // recurses
        }
    }
}
```

**Operand resolution** (variable-or-literal) — [src/misc/SymbolTable.cpp:23-29](src/misc/SymbolTable.cpp#L23-L29)

```cpp
uint16_t InstructionParser::resolveValue(const std::string& operand) {
    if (operand.empty()) return 0;
    if (std::isdigit((unsigned char)operand[0]))      // looks numeric → literal
        return static_cast<uint16_t>(std::stoul(operand));
    return symbolTable.getSymbol(operand);            // else → variable lookup
}
```

**Arithmetic with overflow/underflow safety** — [src/misc/SymbolTable.cpp:110-117](src/misc/SymbolTable.cpp#L110-L117)

```cpp
uint16_t InstructionParser::add(uint16_t a, uint16_t b) {
    uint32_t r = (uint32_t)a + (uint32_t)b;
    return static_cast<uint16_t>(std::min(r, (uint32_t)UINT16_MAX));  // clamp to uint16 max
}
uint16_t InstructionParser::subtract(uint16_t a, uint16_t b) {
    return a >= b ? a - b : 0;                                        // clamp at 0
}
```

> There is also a **text parser** for user-supplied program files
> (`customInstructions` / `parseSingleInstruction` in
> [src/misc/Helper.cpp:93-174](src/misc/Helper.cpp#L93-L174)) that turns lines like
> `FOR([PRINT(x), ADD(x,x,1)], 5)` into the same `Instruction` tree —
> comma-splitting that respects quotes and nested brackets.

**Talking points**
- Two interpreters, one tree-walking pattern.
- `SLEEP` and `FOR` advance the simulated clock (`tick`) so timing matches CPU
  cycles, and check `running` so a process can be stopped mid-program.

---

## 4. Process Representation

**Idea.** A process bundles its scheduling metadata, its program (instruction
tree), its private variable store (symbol table), and its captured output. State
is modeled as a finite set of lifecycle stages.

**Lifecycle states** — [include/misc/Process.hpp:15-21](include/misc/Process.hpp#L15-L21)

```cpp
enum class ProcessState { NEW, READY, RUNNING, WAITING, FINISHED };
```

**Process control block (`ProcessInfo`)** — [include/misc/Process.hpp:23-59](include/misc/Process.hpp#L23-L59)

```cpp
struct ProcessInfo {
    int pid;
    int coreId;                          // which CPU core owns it
    std::string name;
    CPUTick arrivalTime, burstTime, startTime, endTime;  // CPUTick = uint64_t
    int currentLineIndex;                // program counter
    int totalLines;
    ProcessState state;
    std::vector<Instruction> instructions;  // the program
    SymbolTable symbolTable;                 // per-process variables
    std::vector<std::string> logs;
};
```

**The `Process` class** — getters/setters over `ProcessInfo`, plus thread-safe
output capture — [include/misc/Process.hpp:61-131](include/misc/Process.hpp#L61-L131),
[src/misc/Process.cpp:85-102](src/misc/Process.cpp#L85-L102)

```cpp
void Process::appendOutput(const std::string& line) {
    std::string timeStr = Helper::getFormattedTime("(%m/%d/%Y %I:%M:%S%p)");
    std::ostringstream entry;
    entry << timeStr << " Core:" << info.coreId << " \"" << line << "\"";
    std::unique_lock lock(outputMutex);          // guarded: cores write concurrently
    outputLog.push_back(entry.str());
}
```

The symbol table is the process's private memory — auto-initializing on read —
[src/misc/SymbolTable.cpp:6-12](src/misc/SymbolTable.cpp#L6-L12):

```cpp
uint16_t SymbolTable::getSymbol(std::string name) { return variableTable[name]; } // 0 if unseen
void SymbolTable::setSymbol(std::string name, uint16_t value) { variableTable[name] = value; }
```

**Talking points**
- This is effectively a Process Control Block (PCB): identity (`pid`, `name`),
  scheduling (`coreId`, timestamps, `state`), execution context
  (`currentLineIndex` = program counter, `instructions`, `symbolTable`), and I/O
  (`outputLog`).
- `currentLineIndex` is the resume point — critical for round-robin preemption
  (§5), where a process leaves the core and must continue later.
- Output is mutex-guarded because the owning core thread writes while the UI
  thread (`process-smi` / `report-util`) reads.

---

## 5. Scheduler Implementation

**Idea.** `SchedulerService` owns a pool of **CPU-core threads**, a **per-core
ready queue**, and a **process generator thread**. It supports two algorithms —
**FCFS** and **Round-Robin (RR)** — selected from `config.txt`.

**Configuration** — `config.txt`

```
num-cpu 4          scheduler "fcfs"     quantum-cycles 5
batch-process-freq 1   min-ins 100   max-ins 200   delay-per-exec 0
```

**Bringing cores online** — [src/services/SchedulerService.cpp:28-48](src/services/SchedulerService.cpp#L28-L48)

```cpp
void SchedulerService::initScheduler() {
    if (running) return;
    ConfigService cfg; cfg.parseConfigFile();
    config = cfg.getConfig();

    SystemState::getInstance().initializeCores(config.cpuCount);
    cpuReadyQueues.resize(config.cpuCount);   // one ready queue per core
    running = true;

    for (int i = 0; i < (int)config.cpuCount; ++i)
        cpuThreads.emplace_back(&SchedulerService::runCpuCore, this, i);  // spawn workers
}
```

**Process generation** (`scheduler-start`) — batches new dummy processes on a
tick interval — [src/services/SchedulerService.cpp:214-243](src/services/SchedulerService.cpp#L214-L243)

```cpp
void SchedulerService::generateProcessor() {
    while (generating) {
        uint64_t tick = SystemState::getInstance().getSystemTime();
        if (tick % config.batchProcessFreq == 0) {
            int pid = processCounter++;
            ProcessInfo info; info.pid = pid; info.name = /* "pNN" */;
            info.state = ProcessState::NEW;
            info.instructions = generateInstructions(info.name); // random program
            info.totalLines   = info.instructions.size();
            enqueueProcess(info);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
```

**Queue assignment — the algorithm switch** — [src/services/SchedulerService.cpp:158-186](src/services/SchedulerService.cpp#L158-L186)

```cpp
void SchedulerService::enqueueProcess(ProcessInfo info) {
    std::unique_lock lock(queueMutex);

    if (config.schedulingAlgo == "rr") {
        info.coreId = nextCoreAssignment;                       // round-robin spread
        nextCoreAssignment = (nextCoreAssignment + 1) % config.cpuCount;
    }
    else if (config.schedulingAlgo == "fcfs") {
        int shortestCore = 0; size_t minSize = SIZE_MAX;        // load-balance: shortest queue
        for (int i = 0; i < (int)cpuReadyQueues.size(); ++i)
            if (cpuReadyQueues[i].size() < minSize) { minSize = cpuReadyQueues[i].size(); shortestCore = i; }
        info.coreId = shortestCore;
    }

    Process process(info);
    cpuReadyQueues[info.coreId].push(process);
    SystemState::getInstance().addProcess(process);
}
```

**The core worker — FCFS vs RR via time quantum** — [src/services/SchedulerService.cpp:246-319](src/services/SchedulerService.cpp#L246-L319)

```cpp
void SchedulerService::runCpuCore(int coreId) {
    while (running) {
        // 1. dequeue a process from this core's ready queue (locked)
        int pid = -1; bool has = false;
        { std::unique_lock lock(queueMutex);
          if (!cpuReadyQueues[coreId].empty()) {
              pid = cpuReadyQueues[coreId].front().getPid();
              cpuReadyQueues[coreId].pop(); has = true; } }

        if (has && pid >= 0) {
            auto process = SystemState::getInstance().getProcessByPid(pid);
            SystemState::getInstance().setCoreActive(coreId, true);
            process->setState(ProcessState::RUNNING);

            InstructionParser parser(process->getSymbolTable(), running, process->getName());
            const auto& instructions = process->getInstructions();

            // 2. RR → run at most `quantumCycles` lines; FCFS → run all
            int quantum = (config.schedulingAlgo == "rr")
                          ? config.quantumCycles : (int)instructions.size();
            int startIndex = process->getCurrentLineIndex();   // resume point
            int executed = 0;

            for (int i = startIndex; i < (int)instructions.size() && running; ++i) {
                uint64_t tick = SystemState::getInstance().getSystemTime();
                parser.executeBlock({instructions[i]}, tick);  // run one line
                process->setCurrentLineIndex(i + 1);           // advance PC
                if (config.delayPerSec > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(config.delayPerSec));
                if (++executed >= quantum) break;              // 3. quantum expired (RR)
            }

            // 4. preempt or finish
            if (process->getCurrentLineIndex() < (int)instructions.size()) {
                process->setState(ProcessState::READY);
                std::unique_lock lock(queueMutex);
                cpuReadyQueues[coreId].push(*process);         // requeue remainder
            } else {
                process->setState(ProcessState::FINISHED);
                process->setEndTime(SystemState::getInstance().getSystemTime());
            }
            SystemState::getInstance().setCoreActive(coreId, false);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // idle spin
        }
    }
}
```

**How the two algorithms differ (same code path)**

| | FCFS | Round-Robin |
|---|---|---|
| Queue assignment | shortest-queue load balance | rotate `nextCoreAssignment` |
| `quantum` | full program length | `config.quantumCycles` (5) |
| Preemption | none — runs to completion | yes — requeued after quantum, resumes at `currentLineIndex` |

**Concurrency model**
- One thread per CPU core + one generator thread + the main REPL thread.
- `queueMutex` guards all ready-queue access; `running`/`generating` are
  `std::atomic<bool>` flags for cooperative shutdown.
- `processCounter` / `nextCoreAssignment` are atomics (written by the generator,
  read by cores).
- Preemption is *cooperative*: the worker itself stops at the quantum and pushes
  the unfinished process back, relying on `currentLineIndex` as the saved PC.

**Talking points**
- The elegant bit: FCFS and RR share one loop; the only knob is `quantum`
  (= program length vs. `quantumCycles`).
- `report-util` / `process-smi` read live scheduler state from the `SystemState`
  singleton to render utilization and per-process progress (`cur / total`).

---

## Quick demo script

```
initialize            # load config.txt, start clock, bring 4 cores online
scheduler-start       # generator begins batching random processes
report-util           # snapshot: CPU util, cores, running/finished tables
scheduler-stop        # stop generating new processes
exit                  # stop the clock & worker threads, quit
```
