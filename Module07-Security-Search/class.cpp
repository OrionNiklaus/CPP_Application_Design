#include <iostream>
#include <string>
#include "class.h"
using namespace std;

// static member definitions
string User::loggedInUsers[MAX_USERS];
int User::loggedInCount = 0;

// constructor "logs user on"
// inserts the name into the array in alphabetical order
User::User(string name) {
  username = name;
  loggedIn = false;

  // checks for availability to log in
  if (loggedInCount >= MAX_USERS) {
    cout << "Login failed: user list is full. " << username << " not logged in." << endl;
    return;
  }

  // checks for duplicate logins
  if (binarySearch(username) != -1) {
    cout << "Login failed: " << username << " is already logged in." << endl;
    return;
  }


  // construct array to be in alphabetical order
  int i = loggedInCount - 1;
  while (i >= 0 && loggedInUsers[i] > username) {
    loggedInUsers[i + 1] = loggedInUsers[i];
    i--;
  }
  loggedInUsers[i + 1] = username;
  loggedInCount++;
  loggedIn = true;

  cout << username << " logged in." << endl;
}

// destructor logs the user out
User::~User() {
  if (loggedIn && removeFromList(username)) {
    cout << username << " logged out." << endl;
  }
}

// binary search the user array
int User::binarySearch(string name) {
  int low = 0;
  int high = loggedInCount - 1;

  while (low <= high) {
    int mid = low + (high - low) / 2;

    if (loggedInUsers[mid] == name) {
      return mid;
    }
    else if (loggedInUsers[mid] < name) {
      low = mid + 1;
    }
    else {
      high = mid - 1;
    }
  }

  return -1;
}

// removes user and array maintenance
bool User::removeFromList(string name) {
  int index = binarySearch(name);

  if (index == -1) {
    return false;
  }

  for (int i = index; i < loggedInCount - 1; i++) {
    loggedInUsers[i] = loggedInUsers[i + 1];
  }
  loggedInCount--;
  loggedInUsers[loggedInCount] = "";

  return true;
}

// get username
string User::getName() {
  return username;
}

// display user info
void User::displayInfo() {
  cout << "Username: " << username << " (User)" << endl;
  cout << endl;
}

// list logged in users
void User::displayLoggedIn() {
  cout << "Logged in users (" << loggedInCount << "):" << endl;
  for (int i = 0; i < loggedInCount; i++) {
    cout << "  " << i + 1 << ". " << loggedInUsers[i] << endl;
  }
  cout << endl;
}

// admin constructor
Admin::Admin(string name) : User(name) {}

// admin destructor 
// base class logs out
Admin::~Admin() {
  cout << "Admin " << username << " session closed." << endl;
}

// admin version of function
void Admin::displayInfo() {
  cout << "Username: " << username << " (Admin)" << endl;
  cout << endl;
}

// admin only: find user with binary search
bool Admin::findUser(string name) {
  int index = binarySearch(name);

  if (index == -1) {
    cout << "Admin " << username << ": " << name << " is not logged in." << endl;
    cout << endl;
    return false;
  }

  cout << "Admin " << username << ": found " << name << " at position " << index + 1 << "." << endl;
  cout << endl;
  return true;
}

// admin only: remove user from user array
bool Admin::removeUser(string name) {
  if (name == username) {
    cout << "Admin " << username << ": cannot remove yourself." << endl;
    cout << endl;
    return false;
  }

  if (!removeFromList(name)) {
    cout << "Admin " << username << ": cannot remove " << name << ", not logged in." << endl;
    cout << endl;
    return false;
  }

  cout << "Admin " << username << ": removed " << name << " from logged in users." << endl;
  cout << endl;
  return true;
}
