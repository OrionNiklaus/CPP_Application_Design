#include <iostream>
#include <string>
#include "class.h"
using namespace std;

// default constructor implementation
Robot::Robot() {
  SerialNo = 0;
  Name = "";
  CmdSpeed = 0.0;
}

// parameterized constructor implementation
Robot::Robot(int serialNo, string name, double cmdSpeed) {
  SerialNo = serialNo;
  Name = name;
  CmdSpeed = cmdSpeed;
  CurrSpeed = CmdSpeed;
}

// class methods -- getter and setter functions
void Robot::displayStats() {
  cout << "This robot's name is: " << Name << endl;
  cout << "This robot's serial number is: " << SerialNo << endl;
  cout << "The current speed is: " << CurrSpeed << "m/s." << endl;
  cout << " " << endl;
}

void Robot::setSpeed(double cmdSpeed) {
  CurrSpeed = cmdSpeed;
  cout << Name << " speed set to: " << CurrSpeed << " m/s." << endl;
  cout << " " << endl;
}

void Robot::getSpeed() {
  cout << "The current speed of " << Name << " is: " << CurrSpeed << "m/s." << endl;
  cout << " " << endl;
}

