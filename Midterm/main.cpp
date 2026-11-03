#include <iostream>
#include <string>
#include <limits>
#include "Hexapod.h"
using namespace std;

const string CONFIG_FILE = "servo_config.txt";

// read input from user

int readInt(string prompt) {
  int value;
  cout << prompt;
  while (!(cin >> value)) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Please enter a number: ";

  }
  return value;
}

void displayMenu() {
  cout << " --- Hexapod Calibration Tool --- " << endl;
  cout << "1. Show all parts" << endl;
  cout << "2. Show legs" << endl;
  cout << "3. Find a servo by ID number" << endl;
  cout << "4. Adjust servo trim" << endl;
  cout << "5. Toggle a foot switch" << endl;
  cout << "6. Ground contact report" << endl;
  cout << "7. Data type sizes" << endl;
  cout << "8. Save calibration" << endl;
  cout << "0. Exit" << endl;
}

int main() {

  // create robot and load parts from file
  Hexapod robot;
  if (!robot.loadConfig(CONFIG_FILE)) {
    cout << "Could not load calibration. Exiting." << endl;
    return 1;
  }

  int choice = -1;
  while (choice != 0) {
    displayMenu();
    choice = readInt("Enter selection: ");
    cout << endl;

    switch (choice) {
      case 1:
        robot.displayAllParts();
        break;
      
      case 2:
        robot.displayLegs();
        break;
      
      case 3:
      {
        int id = readInt("Enter servo ID (0-17): ");
        Servo * servo = robot.findServo(id);
        if (servo == nullptr) {
          cout << "Servo " << id << " not found." << endl;
        } else {
          servo->display();
        }
        cout << endl;
        break;
      }

      case 4:
      {
        int id = readInt("Enter servo ID (0-17): ");
        Servo * servo = robot.findServo(id);
        if (servo == nullptr) {
          cout << "Servo " << id << " not found." << endl;
          cout << endl;
          break;
        }

        servo->display();
        int newTrim = readInt("Enter new trim in microseconds (-200 to 200): ");
        if (servo->setTrim(newTrim)) {
          cout << "Trim updated." << endl;
          servo->display();
        } else {
          cout << "Trim rejected: out of range for this servo." << endl;
        }
        cout << endl;
        break;
      }

      case 5:
      {
        int id = readInt("Enter foot switch ID (0-5): ");
        Footswitch * foot = robot.findSwitch(id);
        if (foot == nullptr) {
          cout << "Foot switch " << id << " not found." << endl;
        } else {
          foot->setPressed(!foot->isPressed());
          foot->display();
        }
        cout << endl;
        break;
      }

      case 6:
        robot.displayGroundContact();
        break;

      case 7:
        robot.printDataSizes();
        break;

      case 8:
        robot.saveConfig(CONFIG_FILE);
        break;

      case 0:
        cout << "See you next time!" << endl;
        break;

      default:
        cout << "Please select a valid option." << endl;
        cout << endl;
    }
  }
  return 0;
}
