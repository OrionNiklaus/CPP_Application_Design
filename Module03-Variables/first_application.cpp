#include <iostream>
#include <string>
using namespace std;

int main() {

    double version=1.0;
    string name="default";
    string appName="Hexapod Control Application";

    int choice = 0;
    int sec = 0;
    double speed = 0.0;
    string dir = "up";

    // change motors_ready to test both cases

    bool motors_ready = true;

    cout << "Please enter your name: ";
    cin >> name;
    cout << " ----- " << endl;

    cout << "Welcome to the Application, " << name << "!" << endl;
    cout << "Application Name: " << appName << endl;
    cout << "version Number: " << version << endl;
    cout << " ----- " << endl;

    cout << "Please enter program runtime in seconds: ";
    cin >> sec;
    cout << " ----- " << endl;

    cout << "Last Movement Direction: " << dir << endl;
    cout << "Current Speed: " << to_string(speed) << " (m/s)" << endl;

    cout << "Motors Ready? -- ";
    
    if (motors_ready) {
        cout << "Motors armed." << endl;
    } else {
        cout << "Motors disabled." << endl;
        cout << "See troubleshooting documentation." << endl;
    }

}