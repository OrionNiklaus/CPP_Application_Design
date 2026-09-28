#include <iostream>
#include "functions.h"
#include <cstdio>
using namespace std;

// create functions to add a record, display records, and calculate
// a simple result -- move functions out.

int main() {

    TargetArray targetCommand;
    targetArrayInit(&targetCommand);
    
    int option = 0;

    cout << "--- Robot Control App ---" << endl;
    cout << " " << endl;
    cout << "This robot travels on a linear rail forward and backward, with an intial home position of 0m." << endl;
    cout << " " << endl;

    while(option != 5) {
        
        cout << "Please review the following options - " << endl;
        cout << "Option 1: Input a target distance to travel in meters (positive/forward or negative/backward) on the rail" << endl;
        cout << "Option 2: View all travel distance target inputs so far" << endl;
        cout << "Option 3: Find out current positon on the track with respect to the home position" << endl;
        cout << "Option 4: Clear the target distance record for this session and return to home position" << endl;
        cout << "Option 5: Exit the program" << endl;
        cout << "Enter selection here: "; 
        cin >> option;
        cout << " " << endl;


        switch(option) {
            case 1: // input travel distance

                addTarget(&targetCommand);
                break; 

           case 2: // view all travel distance command records

                dispRecords(&targetCommand);
                break;

           case 3: // current track position w.r.t initial position 
           {
                double pos = currentPosition(&targetCommand);
                if (pos > 0) {
                    cout << "Your current displacement is " << pos << " meters in front of the home position." << endl;
                } else if (pos == 0) {
                    cout << "You are at the home position." << endl;
                } else {
                    cout << "Your current displacement is " << (pos*-1) << " meters behind the home position." << endl;
                }
                cout << " " << endl;
                break;
            }

           case 4: // clear target travel distance records, reset to home position

                resetRecords(&targetCommand);
                break;

           case 5: // exit program

                cout << "See you next time!" << endl;
                resetRecords(&targetCommand);
                break;

           default:
                cout << "Please select a valid option." << endl;
        }

    }

    return 0;
}