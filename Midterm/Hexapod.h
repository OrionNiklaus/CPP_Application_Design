#pragma once

#include <iostream>
#include "Servo.h"
#include "Leg.h"
#include "Footswitch.h"

const int NUM_LEGS = 6;
const int SERVOS_PER_LEG = 3;

// this class owns all parts and handles the config file
class Hexapod {
private:
    Servo * servos; // dynamic array, sorted by id
    int servoCount;
    Footswitch * switches; // dynamic array, sorted by id
    int switchCount;
    Component ** parts; // dynamic array of base pointers into other arrays
    int partCount;
    Leg legs[NUM_LEGS];

    // private helpers
    void clear();
    void sortServos();
    void sortSwitches();
    bool buildLegsandParts();

public:
    // default constructor and destructor
    Hexapod();
    ~Hexapod();

    // disable copy
    Hexapod(const Hexapod&) = delete;
    Hexapod& operator=(const Hexapod&) = delete;

    // file i/o
    bool loadConfig(std::string filename);
    bool saveConfig(std::string filename) const;

    // binary search
    Servo * findServo(int id);
    Footswitch * findSwitch(int id);

    // display
    void displayAllParts() const;
    void displayLegs() const;
    void displayGroundContact() const;
    void printDataSizes() const;

};


