#include <iostream>
#include <string>

class Robot {
private:
  
  // class members
  int SerialNo;
  std::string Name;
  double CurrSpeed;
  double CmdSpeed;

public:

  // default constructor
  Robot();
 
  // parameterized constructor
  Robot(int serialNo, std::string name, double cmdSpeed);

  // no destructor needed
  
  // class methods
  void displayStats();
  void setSpeed(double cmdSpeed);
  void getSpeed();
};
