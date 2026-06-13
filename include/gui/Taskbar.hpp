#pragma once

// Component 2: the Taskbar.
// Fixed bottom panel with three clickable buttons that open the Settings,
// About, and Task Manager windows, plus a system tray (CPU/MEM/clock).
class Taskbar {
public:
    void draw();

private:
    void drawSystemTray();
};
