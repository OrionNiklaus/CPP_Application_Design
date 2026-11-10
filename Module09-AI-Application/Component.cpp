#include "Component.h"
using namespace std;

// default constructor
Component::Component() {
    id = 0;
    name = "";
}

// parameterized constructor
Component::Component(uint8_t id, string name) {
    this->id = id;
    this->name = name;
}

// destructor
Component::~Component() {}

// getters
uint8_t Component::getId() const {
    return id;
}

string Component::getName() const {
    return name;
}
