#pragma once

class Core {
public:
    Core(int id);

    int getId() const;
    bool isActive() const;
    void setActive(bool active);

private:
    int id;
    bool active;
};
