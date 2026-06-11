#include "TaskManagerUI.hpp"

#include <algorithm>
#include <cmath>

const char* TaskManagerUI::getStateName(ProcessState state) {
    switch (state) {
        case ProcessState::RUNNING:    return "Running";
        case ProcessState::READY:      return "Ready";
        case ProcessState::WAITING:    return "Waiting";
        case ProcessState::TERMINATED: return "Terminated";
        default:                       return "Unknown";
    }
}

ImVec4 TaskManagerUI::getStateColor(ProcessState state) {
    switch (state) {
        case ProcessState::RUNNING:    return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        case ProcessState::READY:      return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        case ProcessState::WAITING:    return ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // Orange
        case ProcessState::TERMINATED: return ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray
        default:                       return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
    }
}

TaskManagerUI::TaskManagerUI()
    : AWindow("Task Manager"),
      cpuHistory(90, 0.0f),
      memoryHistory(90, 0.0f) {
    // Seed a placeholder process table with dummy values.
    processes = {
        {1,    "kernel_task",   ProcessState::RUNNING,    12.4f,  148213},
        {42,   "csopesy_gui",   ProcessState::RUNNING,    37.5f,   65540},
        {108,  "scheduler",     ProcessState::READY,       4.1f,   20480},
        {256,  "process_p01",   ProcessState::WAITING,     0.0f,    8192},
        {257,  "process_p02",   ProcessState::READY,       2.7f,    8192},
        {310,  "report_util",   ProcessState::RUNNING,     8.9f,   16384},
        {512,  "idle",          ProcessState::TERMINATED,  0.0f,    1024},
    };
}

void TaskManagerUI::draw() {
    if (!this->beginWindow()) return;

    if (ImGui::BeginTabBar("TaskManagerTabs")) {
        if (ImGui::BeginTabItem("Performance")) {
            drawPerformanceTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Processes")) {
            drawProcessesTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    this->endWindow();
}

void TaskManagerUI::drawPerformanceTab() {
    // Refresh the sliding-window buffers before plotting.
    updatePerformanceData();

    ImGui::Text("CPU Usage");
    ImGui::PlotLines("##CPU", cpuHistory.data(), static_cast<int>(cpuHistory.size()),
                     0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));

    ImGui::Spacing();

    ImGui::Text("Memory Usage");
    ImGui::PlotLines("##Memory", memoryHistory.data(), static_cast<int>(memoryHistory.size()),
                     0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
}

void TaskManagerUI::updatePerformanceData() {
    ++tick;

    // Deterministic dummy waveforms so the graphs visibly animate each frame.
    float t = static_cast<float>(tick) * 0.05f;
    float cpu = 50.0f + 35.0f * std::sin(t) + 8.0f * std::sin(t * 3.7f);
    float mem = 55.0f + 20.0f * std::sin(t * 0.3f);
    cpu = std::clamp(cpu, 0.0f, 100.0f);
    mem = std::clamp(mem, 0.0f, 100.0f);

    // Shift left and append (circular-buffer style from the PDF).
    for (size_t i = 0; i + 1 < cpuHistory.size(); ++i) {
        cpuHistory[i] = cpuHistory[i + 1];
    }
    cpuHistory.back() = cpu;

    for (size_t i = 0; i + 1 < memoryHistory.size(); ++i) {
        memoryHistory[i] = memoryHistory[i + 1];
    }
    memoryHistory.back() = mem;
}

void TaskManagerUI::sortProcesses(ImGuiTableSortSpecs* specs) {
    if (!specs || specs->SpecsCount == 0) return;
    const ImGuiTableColumnSortSpecs& s = specs->Specs[0];
    bool asc = s.SortDirection == ImGuiSortDirection_Ascending;

    std::sort(processes.begin(), processes.end(),
              [&](const Process& a, const Process& b) {
                  int cmp = 0;
                  switch (s.ColumnIndex) {
                      case 0: cmp = (a.pid > b.pid) - (a.pid < b.pid); break;
                      case 1: cmp = a.name.compare(b.name); break;
                      case 2: cmp = static_cast<int>(a.state) - static_cast<int>(b.state); break;
                      case 3: cmp = (a.cpuUsage > b.cpuUsage) - (a.cpuUsage < b.cpuUsage); break;
                      case 4: cmp = (a.memoryUsage > b.memoryUsage) - (a.memoryUsage < b.memoryUsage); break;
                      default: break;
                  }
                  return asc ? cmp < 0 : cmp > 0;
              });
}

void TaskManagerUI::drawProcessesTab() {
    ImGuiTableFlags flags = ImGuiTableFlags_Sortable |
                            ImGuiTableFlags_Resizable |
                            ImGuiTableFlags_Borders |
                            ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("ProcessTable", 5, flags)) {
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("State");
        ImGui::TableSetupColumn("CPU %");
        ImGui::TableSetupColumn("Memory");
        ImGui::TableHeadersRow();

        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs()) {
            if (sortSpecs->SpecsDirty) {
                sortProcesses(sortSpecs);
                sortSpecs->SpecsDirty = false;
            }
        }

        for (const auto& p : processes) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%d", p.pid);

            ImGui::TableNextColumn();
            ImGui::Text("%s", p.name.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(getStateColor(p.state), "%s", getStateName(p.state));

            ImGui::TableNextColumn();
            ImGui::Text("%.1f%%", p.cpuUsage);

            ImGui::TableNextColumn();
            ImGui::Text("%zu KB", p.memoryUsage);
        }

        ImGui::EndTable();
    }
}
