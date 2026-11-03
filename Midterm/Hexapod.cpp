#include <iostream>
#include <fstream>
#include "Hexapod.h"
using namespace std;

// leg names in same order as servo ids:
// leg i uses servos 3i, 3i+1, 3i+2
const string LEG_NAMES[NUM_LEGS] = {
    "Front Left",
    "Middle Left", 
    "Rear Left", 
    "Front Right",
    "Middle Right", 
    "Rear Right"
};

// constructor: default until loadConfig()
Hexapod::Hexapod() {
    servos = nullptr;
    servoCount = 0;
    switches = nullptr;
    switchCount = 0;
    parts = nullptr;
    partCount = 0;
}

// destructor: frees object allocations
Hexapod::~Hexapod() {
    clear();
}

// frees all dynamic memory and resets
void Hexapod::clear() {
    delete[] servos;
    delete[] switches;
    delete[] parts; // frees only pointer array, not parts themselves
    
    servos = nullptr;
    switches = nullptr;
    parts = nullptr;
    servoCount = 0; 
    switchCount = 0;
    partCount = 0;

    // reset leg arrays
    for (int i = 0; i < NUM_LEGS; i++) {
        legs[i] = Leg();
    }
}


// read servo_config.txt into dynamic arrays
bool Hexapod::loadConfig(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error - could not open " << filename << endl;
        return false;
    }

    clear();

    // read through config file to count each type; then size arrays
    int numServos = 0;
    int numSwitches = 0;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line[0] == 'S') {
            numServos++;
        }
        else if (line[0] == 'F') {
            numSwitches++;
        }
    }

    // create arrays
    servos = new Servo[numServos];
    switches = new Footswitch[numSwitches];

    // read through config file again; data into arrays
    file.clear();
    file.seekg(0);

    string type;
    while (file >> type) {
        if (type[0] == '#') {
            getline(file, line); // skip comment line
            continue;
        }

        // data line is a servo
        if (type == "S" && servoCount < numServos) {
            int id, channel, minP, maxP, centerP, trimValue;
            string joint;
            file >> id >> channel >> joint >> minP >> maxP >> centerP >> trimValue;
            
            if (!file) {
                cout << "Error - bad servo line in " << filename << endl;
                clear();
                return false;
            }

            servos[servoCount] = Servo(id, channel, joint, minP, maxP, centerP);
            if (!servos[servoCount].setTrim(trimValue)) {
                cout << "Warning - servo " << id << " trim " << trimValue << " out of range, set to 0." << endl;
            }
            servoCount++;
        }

        // data line is a foot switch
        else if (type == "F" && switchCount < numSwitches) {
            int id, pin, pressed;
            file >> id >> pin >> pressed;
            if (!file) {
                cout << "Error - bad switch line in " << filename << endl;
                clear();
                return false;
            }
            switches[switchCount] = Footswitch(id, pin, pressed == 1);
            switchCount++;
        }
    }
    file.close();

    // sort before pointers are taken
    sortServos();
    sortSwitches();

    if (!buildLegsandParts()) {
        clear();
        return false;
    }

    cout << "Loaded " << servoCount << " servos and " << switchCount << " foot switches from " << filename << endl;
    cout << endl;
    return true;

}

// write current calibration to the file
bool Hexapod::saveConfig(string filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error - could not write " << filename << endl;
        return false;
    }

    file << "# type id channel joint min max center trim" << endl;
    for (int i = 0; i < servoCount; i++) {
        file << "S " << static_cast<int>(servos[i].getId())
             << " " << static_cast<int>(servos[i].getChannel())
             << " " << servos[i].getName()
             << " " << servos[i].getMinPulse()
             << " " << servos[i].getMaxPulse()
             << " " << servos[i].getCenterPulse()
             << " " << servos[i].getTrim() << endl;
    }

    file << "# type id pin pressed" << endl;
    for (int i = 0; i < switchCount; i++) {
        file << "F " << static_cast<int>(switches[i].getId())
            << " " << static_cast<int>(switches[i].getPin())
            << " " << (switches[i].isPressed() ? 1 : 0) << endl;

    }

    file.close();
    cout << "Saved calibration values to " << filename << "." << endl;
    cout << endl;
    return true;
}

// pre-sort for binary search
void Hexapod::sortServos() {
    for (int i = 1; i < servoCount; i++) {
        Servo current = servos[i];
        int j = i-1;
        while (j >= 0 && servos[j].getId() > current.getId()) {
            servos[j+1] = servos[j];
            j--;
        }
        servos[j+1] = current;
    }
}

void Hexapod::sortSwitches() {
    for (int i = 1; i < switchCount; i++) {
        Footswitch current = switches[i];
        int j = i-1;
        while (j >= 0 && switches[j].getId() > current.getId()) {
            switches[j+1] = switches[j];
            j--;
        }
        switches[j+1] = current;
    }
}

// connect leg to parts and fill base pointer array
bool Hexapod::buildLegsandParts() {
    for (int i = 0; i < NUM_LEGS; i++) {
        Servo * coxa = findServo(i * SERVOS_PER_LEG);
        Servo * femur = findServo(i * SERVOS_PER_LEG + 1);
        Servo * tibia = findServo(i * SERVOS_PER_LEG + 2); 
        Footswitch * foot = findSwitch(i);
 
        if (coxa == nullptr || femur == nullptr || tibia == nullptr || foot == nullptr) {
            cout << "Error - config file is missing parts for the " << LEG_NAMES[i] << " leg." << endl;
            return false;
        }

        legs[i].attach(LEG_NAMES[i], coxa, femur, tibia, foot);
    }

    // every part goes into base class pointer array
    partCount = servoCount + switchCount;
    parts = new Component * [partCount];
    for (int i = 0; i < servoCount; i++) {
        parts[i] = &servos[i];
    }
    for (int i = 0; i < switchCount; i++) {
        parts[servoCount+i] = &switches[i];
    }

    return true;
}

// binary search
Servo * Hexapod::findServo(int id) {
    int low = 0;
    int high = servoCount-1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int midId = servos[mid].getId();

        if (midId == id) {
            return &servos[mid];
        } else if (midId < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return nullptr;
}

Footswitch * Hexapod::findSwitch(int id) {
    int low = 0;
    int high = switchCount-1;

    while (low <= high) {
        int mid = low + (high-low) / 2;
        int midId = switches[mid].getId();

        if (midId == id) {
            return &switches[mid];
        } else if (midId < id) {
            low = mid+1;
        } else {
            high = mid-1;
        }
    }
    return nullptr;
}

// polymorphism; one loop, each part runs own display()
void Hexapod::displayAllParts() const {
    cout << "All parts (" << partCount << "):" << endl;
    for (int i = 0; i < partCount; i++) {
        parts[i]->display();
    }
    cout << endl;
}

void Hexapod::displayLegs() const {
    for (int i = 0; i < NUM_LEGS; i++) {
        legs[i].display();
    }
}

// which feet are on the ground
void Hexapod::displayGroundContact() const {
    int feetDown = 0;

    cout << "Ground contact:" << endl;
    for (int i = 0; i < NUM_LEGS; i++) {
        bool down = legs[i].footOnGround();
        if (down) {
            feetDown++;
        }
        cout << " " << legs[i].getName() << ": " << (down ? "DOWN" : "up") << endl;
    }

    cout << feetDown << " of " << NUM_LEGS << " feet on the ground. ";
    if (feetDown >= 3) {
        cout << "Robot can stand." << endl;
    } else {
        cout << "Not enough contact to stand." << endl;
    }
    cout << endl;
}

// method to report bytes each type uses on this machine
void Hexapod::printDataSizes() const {
  cout << "Data type sizes (bytes):" << endl;
  cout << "  bool         " << sizeof(bool) << endl;
  cout << "  uint8_t      " << sizeof(uint8_t) << "   id, channel, pin" << endl;
  cout << "  int16_t      " << sizeof(int16_t) << "   trim (signed)" << endl;
  cout << "  uint16_t     " << sizeof(uint16_t) << "   pulse widths" << endl;
  cout << "  int          " << sizeof(int) << endl;
  cout << "  std::string  " << sizeof(string) << endl;
  cout << "  Component*   " << sizeof(Component*) << "   any pointer" << endl;
  cout << endl;

  cout << "Class sizes (bytes):" << endl;
  cout << "  Servo        " << sizeof(Servo) << endl;
  cout << "  FootSwitch   " << sizeof(Footswitch) << endl;
  cout << "  Leg          " << sizeof(Leg) << endl;
  cout << "  Hexapod      " << sizeof(Hexapod) << endl;
  cout << endl;

  cout << "Dynamic memory in use:" << endl;
  cout << "  servos   " << servoCount << " x " << sizeof(Servo)
       << " = " << servoCount * sizeof(Servo) << " bytes" << endl;
  cout << "  switches " << switchCount << " x " << sizeof(Footswitch)
       << " = " << switchCount * sizeof(Footswitch) << " bytes" << endl;
  cout << "  parts    " << partCount << " x " << sizeof(Component*)
       << " = " << partCount * sizeof(Component*) << " bytes" << endl;
  cout << endl;
}