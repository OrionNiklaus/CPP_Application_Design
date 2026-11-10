#include <iostream>
#include "Leg.h"
#include "Servo.h" // servo methods are called here
#include "Footswitch.h"
using namespace std;

// default constructor
Leg::Leg() {
  name = "";
  coxa = nullptr;
  femur = nullptr;
  tibia = nullptr;
  foot = nullptr;
}

// store pointers to Hexapod owned members
void Leg::attach(string name, Servo * coxa, Servo * femur, Servo * tibia, Footswitch * foot) {
  this->name = name;
  this->coxa = coxa;
  this->femur = femur;
  this->tibia = tibia;
  this->foot = foot;
}

// getters
string Leg::getName() const {
  return name;
}

// returns position state of foot
bool Leg::footOnGround() const {
  return foot != nullptr && foot->isPressed();
}

// calls method through pointer to display leg data
void Leg::display() const {
  if (coxa == nullptr || femur == nullptr || tibia == nullptr || foot == nullptr) {
    cout << "Leg not attached." << endl;
    return;
  }
  
  cout << name << (footOnGround() ? " [on ground]" : " [lifted]") << endl;
  cout << "  ";

  // calling Servo class method display()
  coxa->display(); 
  cout << "  ";
  femur->display();
  cout << "  ";
  tibia->display();
  cout << "  ";
  foot->display();
  cout << endl;
}


