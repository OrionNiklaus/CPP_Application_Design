#pragma once

#include <cstdint>
#include <string>
#include "Component.h"

// Maximum trim allowed
const int MAX_TRIM = 200;

// safe pulse width limits for these servos (microseconds)
const int PULSE_LIMIT_MIN = 500;
const int PULSE_LIMIT_MAX = 2500;

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
  int getMinTrim() const; // smallest trim that keeps center inside the range
  int getMaxTrim() const; // largest trim that keeps center inside the range

  // setter methods
  bool setTrim(int newTrim); // trim starts at zero
  // returns false if the range is invalid; resets trim to 0 if it no longer fits
  bool setPulseRange(int newMin, int newMax, int newCenter);

  // checks a pulse range without needing a Servo object (used by loadConfig too)
  static bool validatePulseRange(int minPulse, int maxPulse, int centerPulse, std::string& error);

  // display implemented from base class
  void display() const override;
};



