#include "../../include/misc/Core.hpp"
#include "Process.hpp"

int Core::getId() const { return id; }
bool Core::isActive() const { return active; }
void Core::setActive(bool act) { active = act; }
void Core::setId(int id) { this->id = id; }
void Core::setActiveProcess(Process *p) {this->process = p;}

