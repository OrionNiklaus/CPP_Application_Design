#include <iostream>
#include <string>

// base class
class User {
protected:
    string username;

public:
    // constructor
    User(string name) : username(name) {}
  
};

// derived class 
class Admin : public User {

    // constructor
    Admin(string name) : User(name) {
    
    // class methods


    }
};

// need destuctors

// create logged in user array

// use binary search
// find logged in users, remove user
