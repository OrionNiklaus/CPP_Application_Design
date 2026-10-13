#include <iostream>
#include <string>
#include "class.h"
using namespace std;

int main() {

  // create objects -- users and admin
  User user1("Charlie");
  User user2("Alice");
  User user3("Dana");
  User user4("Bob");
  Admin admin("Orion");
  cout << endl;

  // display user info
  admin.displayInfo();
  user1.displayInfo();

  /// display logged in user list
  User::displayLoggedIn();

  // duplicate login rejected
  User duplicate("Alice");
  cout << endl;

  // showing all conditions for findUser
  admin.findUser("Dana");
  admin.findUser("Eve");

  // showing all conditions for removeUser
  admin.removeUser("Charlie");
  admin.removeUser("Eve");
  admin.removeUser("Orion");

  User::displayLoggedIn();

  // create object in a block to show destructor function
  {
    User guest("Guest");
    User::displayLoggedIn();
  }
  cout << endl;

  User::displayLoggedIn();

  return 0; // all remaining users logged out

}
