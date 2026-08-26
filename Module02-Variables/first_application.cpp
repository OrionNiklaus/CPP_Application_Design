#include <iostream>
#include <string>
using namespace std;

int main() {
    double version=1.0;
    string name="default";
    string appName="Hexapod Control Application";
    
    cout << "Please enter your name: ";
    cin >> name;
    cout << " ----- " << endl;

    cout << "Welcome to the Application, " << name << "!" << endl;
    cout << "Application Name: " << appName << endl;
    cout << "version Number: " << version << endl;

}