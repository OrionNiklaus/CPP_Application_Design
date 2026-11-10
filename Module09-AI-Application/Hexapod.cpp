#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Hexapod.h"
#include "Validation.h"
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


// split a line into whitespace separated words
static vector<string> splitWords(const string& line) {
    vector<string> words;
    istringstream in(line);
    string word;
    while (in >> word) {
        words.push_back(word);
    }
    return words;
}

// parse one numeric field and check its range; prefixes the error with the field name
static bool readField(const string& text, const string& field, int min, int max, int& value, string& error) {
    if (!parseInt(text, value, error) || !checkRange(value, min, max, error)) {
        error = field + ": " + error;
        return false;
    }
    return true;
}

// read servo_config.txt into dynamic arrays; every line is validated before it is accepted
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
        vector<string> words = splitWords(line);
        if (words.empty()) {
            continue;
        }
        if (words[0] == "S") {
            numServos++;
        } else if (words[0] == "F") {
            numSwitches++;
        }
    }

    // create arrays
    servos = new Servo[numServos];
    switches = new Footswitch[numSwitches];

    // track ids/channels/pins already used to catch duplicates
    const int NUM_SERVOS = NUM_LEGS * SERVOS_PER_LEG;
    const int MAX_GPIO_PIN = 27;
    const string JOINTS[SERVOS_PER_LEG] = {"coxa", "femur", "tibia"};
    bool servoIdUsed[NUM_SERVOS] = {false};
    bool channelUsed[NUM_SERVOS] = {false};
    bool switchIdUsed[NUM_LEGS] = {false};
    bool pinUsed[MAX_GPIO_PIN + 1] = {false};

    // read through config file again; data into arrays
    file.clear();
    file.seekg(0);

    int lineNumber = 0;
    int errorCount = 0;
    while (getline(file, line)) {
        lineNumber++;
        vector<string> words = splitWords(line);
        if (words.empty() || words[0][0] == '#') {
            continue; // blank line or comment
        }

        string error;

        // data line is a servo: S id channel joint min max center trim
        if (words[0] == "S") {
            int id, channel, minP, maxP, centerP, trimValue;
            if (words.size() != 8) {
                error = "servo line needs 8 values: S id channel joint min max center trim";
            } else if (readField(words[1], "servo id", 0, NUM_SERVOS - 1, id, error) &&
                       readField(words[2], "channel", 0, NUM_SERVOS - 1, channel, error) &&
                       readField(words[4], "min pulse", PULSE_LIMIT_MIN, PULSE_LIMIT_MAX, minP, error) &&
                       readField(words[5], "max pulse", PULSE_LIMIT_MIN, PULSE_LIMIT_MAX, maxP, error) &&
                       readField(words[6], "center pulse", PULSE_LIMIT_MIN, PULSE_LIMIT_MAX, centerP, error) &&
                       readField(words[7], "trim", -MAX_TRIM, MAX_TRIM, trimValue, error) &&
                       Servo::validatePulseRange(minP, maxP, centerP, error)) {
                // joint name must match the servo's position in its leg
                string expectedJoint = JOINTS[id % SERVOS_PER_LEG];
                if (words[3] != expectedJoint) {
                    error = "servo " + to_string(id) + " must be a " + expectedJoint + ", not \"" + words[3] + "\"";
                } else if (servoIdUsed[id]) {
                    error = "servo id " + to_string(id) + " is listed more than once";
                } else if (channelUsed[channel]) {
                    error = "channel " + to_string(channel) + " is used by more than one servo";
                } else {
                    servoIdUsed[id] = true;
                    channelUsed[channel] = true;
                    servos[servoCount] = Servo(id, channel, words[3], minP, maxP, centerP);
                    if (!servos[servoCount].setTrim(trimValue)) {
                        cout << "Warning - " << filename << " line " << lineNumber << ": servo " << id
                             << " trim " << trimValue << " moves center outside its range, set to 0." << endl;
                    }
                    servoCount++;
                }
            }
        }

        // data line is a foot switch: F id pin pressed
        else if (words[0] == "F") {
            int id, pin, pressed;
            if (words.size() != 4) {
                error = "switch line needs 4 values: F id pin pressed";
            } else if (readField(words[1], "switch id", 0, NUM_LEGS - 1, id, error) &&
                       readField(words[2], "GPIO pin", 0, MAX_GPIO_PIN, pin, error) &&
                       readField(words[3], "pressed (0 or 1)", 0, 1, pressed, error)) {
                if (switchIdUsed[id]) {
                    error = "switch id " + to_string(id) + " is listed more than once";
                } else if (pinUsed[pin]) {
                    error = "GPIO pin " + to_string(pin) + " is used by more than one switch";
                } else {
                    switchIdUsed[id] = true;
                    pinUsed[pin] = true;
                    switches[switchCount] = Footswitch(id, pin, pressed == 1);
                    switchCount++;
                }
            }
        }

        else {
            error = "unknown line type \"" + words[0] + "\" (expected S, F, or # for a comment)";
        }

        if (!error.empty()) {
            cout << "Error - " << filename << " line " << lineNumber << ": " << error << endl;
            errorCount++;
        }
    }
    file.close();

    if (errorCount > 0) {
        cout << errorCount << " problem(s) found in " << filename << ". Fix the file and try again." << endl;
        clear();
        return false;
    }

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
// technician and date are validated by the caller before saving
bool Hexapod::saveConfig(string filename, string technician, string date) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error - could not write " << filename << endl;
        return false;
    }

    file << "# calibrated by " << technician << " on " << date << endl;
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

std::string Hexapod::getLegName(int legIndex) const {
    if (legIndex < 0 || legIndex >= NUM_LEGS) {
        return "Unknown leg";
    }
    return legs[legIndex].getName();
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