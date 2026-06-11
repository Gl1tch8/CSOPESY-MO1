#pragma once

// Component 1: the Desktop (see imgui.pdf "The Desktop").
// Full-screen base layer rendered first each frame: gradient wallpaper,
// a live clock, and a PWR (shutdown) button that is the only way to quit.
class Desktop {
public:
    void draw();

    // True once the user has clicked the PWR button this session.
    bool shutdownRequested() const { return powerOff; }

private:
    void drawWallpaper();
    void drawClock();
    void drawPowerButton();
    void drawLauncher();

    bool powerOff = false;
};
