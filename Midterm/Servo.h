#pragma once

#include <cstdint>
#include <string>
#include "Component.h"

// Maximum trim allowed
const int MAX_TRIM = 200;

class Servo : public Component {
private:
  uint8_t channel; // on servo board
  uint16_t minPulse, maxPulse, centerPulse;
  int16_t trim;

public:
  Servo();
  Servo(uint8_t id, uint8_t channel, std::string joint, uint16_t minPulse, uint16_t maxPulse, uint16_t centerPulse);

  // getter methods
  uint8_t getChannel() const;
  uint16_t getMinPulse() const;
  uint16_t getMaxPulse() const;
  uint16_t getCenterPulse() const;
  int16_t getTrim() const;
  uint16_t getTrimmedCenter() const;

  // setter methods
  bool setTrim(int newTrim); // trim starts at zero

  // display implemented from base class
  void display() const override;
};



