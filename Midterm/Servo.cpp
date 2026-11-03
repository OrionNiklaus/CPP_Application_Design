#include <iostream>
#include "Servo.h"
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

// print servo information -- static cast so values print as number
void Servo::display() const {
    cout << "Servo " << static_cast<int>(id) << " (" << name << ")"
         << " | channel " << static_cast<int>(channel)
         << " | range " << minPulse << "-" << maxPulse << " us"
         << " | center " << centerPulse << " us"
         << " | trim " << trim << " us"
         << " -> " << getTrimmedCenter() << " us" << endl;
}
