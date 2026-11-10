#include <iostream>
#include "Servo.h"
#include "Validation.h"
using namespace std;

// default constructor
Servo::Servo() : Component() {
    channel = 0;
    minPulse = 0;
    maxPulse = 0;
    centerPulse = 0;
    trim = 0;
}

// parameterized constructor
Servo::Servo(uint8_t id, uint8_t channel, string joint, uint16_t minPulse, uint16_t maxPulse, uint16_t centerPulse) : Component(id, joint) {
    this->channel = channel;
    this->minPulse = minPulse;
    this->maxPulse = maxPulse;
    this->centerPulse = centerPulse;
    trim = 0;
}

// getter definitions
uint8_t Servo::getChannel() const {
    return channel;
}

uint16_t Servo::getMinPulse() const {
    return minPulse;
}

uint16_t Servo::getMaxPulse() const {
    return maxPulse;
}

uint16_t Servo::getCenterPulse() const {
    return centerPulse;
}

int16_t Servo::getTrim() const {
    return trim;
}

// pulse sent for "center" position after calibration
uint16_t Servo::getTrimmedCenter() const {
    return static_cast<uint16_t>(centerPulse + trim);
}

// trim limits: +/- MAX_TRIM, but center + trim must stay inside min-max
int Servo::getMinTrim() const {
    int limit = static_cast<int>(minPulse) - static_cast<int>(centerPulse);
    return limit > -MAX_TRIM ? limit : -MAX_TRIM;
}

int Servo::getMaxTrim() const {
    int limit = static_cast<int>(maxPulse) - static_cast<int>(centerPulse);
    return limit < MAX_TRIM ? limit : MAX_TRIM;
}

// set trim; safeguards for trim size
bool Servo::setTrim(int newTrim) {
    if (newTrim < -MAX_TRIM || newTrim > MAX_TRIM) {
        return false;
    }
    
    int result = centerPulse + newTrim;
    if (result < minPulse || result > maxPulse) {
        return false;
    }

    trim = static_cast<int16_t>(newTrim);
    return true;
}

// each pulse inside the hardware limits, and min < center < max
bool Servo::validatePulseRange(int minP, int maxP, int centerP, string& error) {
    if (!checkRange(minP, PULSE_LIMIT_MIN, PULSE_LIMIT_MAX, error)) {
        error = "min pulse: " + error;
        return false;
    }
    if (!checkRange(maxP, PULSE_LIMIT_MIN, PULSE_LIMIT_MAX, error)) {
        error = "max pulse: " + error;
        return false;
    }
    if (!checkRange(centerP, PULSE_LIMIT_MIN, PULSE_LIMIT_MAX, error)) {
        error = "center pulse: " + error;
        return false;
    }
    if (minP >= maxP) {
        error = "min pulse (" + to_string(minP) + ") must be less than max pulse (" + to_string(maxP) + ").";
        return false;
    }
    if (centerP <= minP || centerP >= maxP) {
        error = "center pulse (" + to_string(centerP) + ") must be between min and max.";
        return false;
    }
    return true;
}

bool Servo::setPulseRange(int newMin, int newMax, int newCenter) {
    string error;
    if (!validatePulseRange(newMin, newMax, newCenter, error)) {
        return false;
    }
    minPulse = static_cast<uint16_t>(newMin);
    maxPulse = static_cast<uint16_t>(newMax);
    centerPulse = static_cast<uint16_t>(newCenter);

    // old trim may push center outside the new range
    if (trim < getMinTrim() || trim > getMaxTrim()) {
        trim = 0;
    }
    return true;
}

// print servo information -- static cast so values print as number
void Servo::display() const {
    cout << "Servo " << static_cast<int>(id) << " (" << name << ")"
         << " | channel " << static_cast<int>(channel)
         << " | range " << minPulse << "-" << maxPulse << " us"
         << " | center " << centerPulse << " us"
         << " | trim " << trim << " us"
         << " -> " << getTrimmedCenter() << " us" << endl;
}
