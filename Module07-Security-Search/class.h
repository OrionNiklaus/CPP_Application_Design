#pragma once
#include <iostream>
#include <string>

// max number of users
const int MAX_USERS = 10;

// base class
class User {
protected:
  std::string username; // name for user array
  bool loggedIn; // login/logout flag

  // kept in alphabetical order so binary search works
  static std::string loggedInUsers[MAX_USERS]; // logged in user array
  static int loggedInCount; // number of users logged in

  // binary search; returns index of name or -1 if not found
  static int binarySearch(std::string name);

  // remove name from the array; returns true if removed
  static bool removeFromList(std::string name);

public:
  // constructor; adds name to user array
  User(std::string name);

  // destructor; removes name from user array
  virtual ~User();

  // class methods
  std::string getName();
  virtual void displayInfo();
  static void displayLoggedIn();
};

// derived class
class Admin : public User {
public:
  // constructor
  Admin(std::string name);

  // destructor
  ~Admin();

  // class methods
  void displayInfo() override;
  bool findUser(std::string name);
  bool removeUser(std::string name);
};
