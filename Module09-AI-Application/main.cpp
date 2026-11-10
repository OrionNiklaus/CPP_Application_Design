#include <iostream>
#include <string>
#include "Hexapod.h"
#include "Validation.h"
#include "AnomalyDetector.h"
using namespace std;

const string CONFIG_FILE = "servo_config.txt";
const int MAX_SERVO_ID = NUM_LEGS * SERVOS_PER_LEG - 1;
const int MAX_SWITCH_ID = NUM_LEGS - 1;
const int MAX_MENU_CHOICE = 10;

void displayMenu() {
  cout << " --- Hexapod Calibration Tool --- " << endl;
  cout << "1. Show all parts" << endl;
  cout << "2. Show legs" << endl;
  cout << "3. Find a servo by ID number" << endl;
  cout << "4. Adjust servo trim" << endl;
  cout << "5. Edit servo pulse range" << endl;
  cout << "6. Toggle a foot switch" << endl;
  cout << "7. Ground contact report" << endl;
  cout << "8. Data type sizes" << endl;
  cout << "9. Save calibration" << endl;
  cout << "10. AI calibration health check" << endl;
  cout << "0. Exit" << endl;
}

// ask for a servo id until it is in range, then look it up
Servo * askForServo(Hexapod& robot) {
  int id = readIntInRange("Enter servo ID (0-" + to_string(MAX_SERVO_ID) + "): ", 0, MAX_SERVO_ID);
  Servo * servo = robot.findServo(id);
  if (servo == nullptr) {
    cout << "Servo " << id << " not found." << endl;
  }
  return servo;
}

void adjustTrim(Hexapod& robot) {
  Servo * servo = askForServo(robot);
  if (servo == nullptr) {
    return;
  }
  servo->display();

  // allowed range depends on this servo's pulse limits, not just +/- MAX_TRIM
  int low = servo->getMinTrim();
  int high = servo->getMaxTrim();
  int newTrim = readIntInRange("Enter new trim in microseconds (" + to_string(low) + " to " + to_string(high) + "): ", low, high);

  if (servo->setTrim(newTrim)) {
    cout << "Trim updated." << endl;
    servo->display();
  } else {
    cout << "Trim rejected: out of range for this servo." << endl;
  }
}

void editPulseRange(Hexapod& robot) {
  Servo * servo = askForServo(robot);
  if (servo == nullptr) {
    return;
  }
  servo->display();

  // each prompt's range depends on the previous answer so min < center < max
  int minP = readIntInRange("New min pulse (" + to_string(PULSE_LIMIT_MIN) + "-" + to_string(PULSE_LIMIT_MAX - 2) + " us): ",
                            PULSE_LIMIT_MIN, PULSE_LIMIT_MAX - 2);
  int maxP = readIntInRange("New max pulse (" + to_string(minP + 2) + "-" + to_string(PULSE_LIMIT_MAX) + " us): ",
                            minP + 2, PULSE_LIMIT_MAX);
  int centerP = readIntInRange("New center pulse (" + to_string(minP + 1) + "-" + to_string(maxP - 1) + " us): ",
                               minP + 1, maxP - 1);

  int oldTrim = servo->getTrim();
  if (!servo->setPulseRange(minP, maxP, centerP)) {
    cout << "Pulse range rejected." << endl;
    return;
  }
  cout << "Pulse range updated." << endl;
  if (servo->getTrim() != oldTrim) {
    cout << "Note: old trim of " << oldTrim << " us no longer fit the new range and was reset to 0." << endl;
  }
  servo->display();
}

void toggleSwitch(Hexapod& robot) {
  int id = readIntInRange("Enter foot switch ID (0-" + to_string(MAX_SWITCH_ID) + "): ", 0, MAX_SWITCH_ID);
  Footswitch * foot = robot.findSwitch(id);
  if (foot == nullptr) {
    cout << "Foot switch " << id << " not found." << endl;
  } else {
    foot->setPressed(!foot->isPressed());
    foot->display();
  }
}

void saveCalibration(Hexapod& robot) {
  string technician = readName("Technician name: ");
  string date = readDate("Calibration date (YYYY-MM-DD): ");
  if (readYesNo("Overwrite " + CONFIG_FILE + "? (y/n): ")) {
    robot.saveConfig(CONFIG_FILE, technician, date);
  } else {
    cout << "Save cancelled." << endl;
  }
}

// run the AI feature; main only knows the AnomalyDetector interface
void runHealthCheck(Hexapod& robot, AnomalyDetector& detector) {
  cout << "Analyzing calibration..." << endl << endl;
  HealthReport report = detector.analyze(robot);
  displayReport(report);

  if (readYesNo("Show the data a live AI service would receive? (y/n): ")) {
    cout << endl << buildCalibrationSnapshot(robot);
  }
}

int main() {

  // create robot and load parts from file
  Hexapod robot;

  // swap this for a live AI detector when one is available
  MockAnomalyDetector detector;
  if (!robot.loadConfig(CONFIG_FILE)) {
    cout << "Could not load calibration. Exiting." << endl;
    return 1;
  }

  try {
    int choice = -1;
    while (choice != 0) {
      displayMenu();
      choice = readIntInRange("Enter selection (0-" + to_string(MAX_MENU_CHOICE) + "): ", 0, MAX_MENU_CHOICE);
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
          Servo * servo = askForServo(robot);
          if (servo != nullptr) {
            servo->display();
          }
          cout << endl;
          break;
        }

        case 4:
          adjustTrim(robot);
          cout << endl;
          break;

        case 5:
          editPulseRange(robot);
          cout << endl;
          break;

        case 6:
          toggleSwitch(robot);
          cout << endl;
          break;

        case 7:
          robot.displayGroundContact();
          break;

        case 8:
          robot.printDataSizes();
          break;

        case 9:
          saveCalibration(robot);
          cout << endl;
          break;

        case 10:
          runHealthCheck(robot, detector);
          cout << endl;
          break;

        case 0:
          cout << "See you next time!" << endl;
          break;
      }
    }
  } catch (const InputClosed&) {
    // Ctrl+D or end of piped input: stop instead of looping forever
    cout << endl << "Input closed. Exiting without saving." << endl;
  }
  return 0;
}
