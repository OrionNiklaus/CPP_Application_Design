#pragma once
#include <cstdint>
#include "Component.h"

// micro switch on each foot
class Footswitch : public Component {
private:
  uint8_t pin; // GPIO pin
  bool pressed; // true = foot on the ground

public:
  Footswitch();
  Footswitch(uint8_t id, uint8_t pin, bool pressed);

  // getters
  uint8_t getPin() const;
  bool isPressed() const;

  // setter; simulates foot touching down or lifting off
  void setPressed(bool state); // simulate foot contact

  // Component::display() implementation
  void display() const override;
};
