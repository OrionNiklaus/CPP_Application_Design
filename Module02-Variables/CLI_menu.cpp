#include <iostream>
#include <string>
using namespace std;

void std_gait(int sec) {
    cout << "Standard gait pattern initiated." << endl;
    cout << "Program running for " << sec << " seconds." << endl;
    cout << endl;
    cout << " --- " << endl;
}

void wave_gait(int sec) {
    cout << "Wave gait pattern initiated." << endl;
    cout << "Program running for " << sec << " seconds." << endl;
    cout << endl;
    cout << " --- " << endl;
}

void rev_gait(int sec) {
    cout << "Reverse gait pattern initiated." << endl;
    cout << "Program running for " << sec << " seconds." << endl;
    cout << endl;
    cout << " --- " << endl;
}

void dash_gait(int sec) {
    cout << "Dashing gait pattern initiated." << endl;
    cout << "Program running for " << sec << " seconds." << endl;
    cout << endl;
    cout << " --- " << endl;
}

void strafe_gait(int sec) {
    cout << "Strafing gait pattern initiated." << endl;
    cout << "Program running for " << sec << " seconds." << endl;
    cout << endl;
    cout << " --- " << endl;
}

int main() {

    int choice = 0;
    int sec = 0;

    cout << "--Hexapod Control Application--" << endl;
    cout << "Please enter program runtime in seconds: ";
    cin >> sec;
    cout << " --- " << endl;

    while (choice != 6) {
        cout << "Please select the gait pattern:" << endl;
        cout << "1. Standard gait pattern." << endl;
        cout << "2. Wave gait pattern." << endl;
        cout << "3. Reverse gait pattern." << endl;
        cout << "4. Dashing gait pattern." << endl;
        cout << "5. Strafing gait pattern." << endl;
        cout << "6. Exit application." << endl;
        cout << " --- " << endl;
        cout << "Please enter your selection: ";
        cin >> choice;
        cout << " " << endl;
        cout << " --- " << endl;

        switch (choice) {
            case 1:
                std_gait(sec);
                break;
            case 2:
                wave_gait(sec);
                break;
            case 3:
                rev_gait(sec);
                break;
            case 4:
                dash_gait(sec);
                break;
            case 5:
                strafe_gait(sec);
                break;
            case 6:
                cout << "See you next time!" << endl;
                break;
            default:
                cout << "Please select a valid option." << endl;
        }   
    }

    return 0;

}
