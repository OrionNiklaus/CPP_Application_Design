#include <string>
#include "class.h"
using namespace std;

int main() {
  
  // Create class objects
  Robot rover(111, "Land Rover", 0.5);
  Robot drone(333, "Drone", 4.0);

  // getter and setter class methods called
  rover.displayStats();
  rover.setSpeed(1.0);
  rover.getSpeed();

  drone.displayStats();

  return 0;

}
