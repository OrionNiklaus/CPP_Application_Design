#pragma once
#include <cstdint> 
#include <string>

// abstract base class for physical hardware components
class Component {
protected:
  uint8_t id; // servos 0-17; foot switches 0-5
  std::string name; // location name ex. "coxa" or "foot"

public:
  // default constructor
  Component();
  // parameterized constructor
  Component(uint8_t id, std::string name);

  // virtual destructor for base class
  virtual ~Component();

  // getters
  uint8_t getId() const;
  std::string getName() const;

  // pure virtual - derived class must define
  virtual void display() const = 0;
};
