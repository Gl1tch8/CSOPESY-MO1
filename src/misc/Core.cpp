#include "../../include/Core.hpp"

Core::Core(int id) : id(id), active(false) {}

int Core::getId() const { return id; }
bool Core::isActive() const { return active; }
void Core::setActive(bool act) { active = act; }
