#include <iostream>
#include "Footswitch.h"
using namespace std;

// default constructor
Footswitch::Footswitch() : Component() {
    pin = 0;
    pressed = false;
}

// parameterized constructor; every switch named "foot"
Footswitch::Footswitch(uint8_t id, uint8_t pin, bool pressed) : Component(id, "foot") {
    this->pin = pin;
    this->pressed = pressed;
}

// getters
uint8_t Footswitch::getPin() const {
    return pin;
}

bool Footswitch::isPressed() const {
    return pressed;
}

// setter
void Footswitch::setPressed(bool state) {
    pressed = state;
}

// Component::display() implementation
void Footswitch::display() const {
    cout << "Switch " << static_cast<int>(id) << " (" << name << ")"
         << " | pin " << static_cast<int>(pin)
         << " | " << (pressed ? "PRESSED" : "released") << endl;
}