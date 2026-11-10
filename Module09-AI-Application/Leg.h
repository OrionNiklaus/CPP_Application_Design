#pragma once
#include <string>

class Servo; // forward declarations of hardware classes
class Footswitch; // leg holds only pointers, full classes aren't needed

class Leg {
private:
  std::string name; // leg position, ex. "Middle Right"
  Servo * coxa; // hip joint, swings side to side
  Servo * femur; // upper leg, swings up and down at "hip"
  Servo * tibia; // lower leg, swings up and down at "knee"
  Footswitch * foot; // contact sensor on foot

public:
  Leg(); // default constructor

  // connect the leg to its servos and foot switch
  void attach(std::string name, Servo * coxa, Servo * femur, Servo * tibia, Footswitch * foot);
  
  // getters
  // asks for footswitch state
  bool footOnGround() const; 
  std::string getName() const;

  // print leg and its hardware
  void display() const;
};
