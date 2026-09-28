#include "functions.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

void targetArrayInit(TargetArray *array) {
    array->data = NULL;
    array->moveCount = 0; 
    array->capacity = 0;
}

double appendRecord(TargetArray *array, double xTarget) {
    // reallocate more dynamic memory if at capacity
    if (array->moveCount == array->capacity) { 
        size_t newCapacity = (array->capacity == 0) ? 4 : array->capacity * 2;
        double *tmp = static_cast<double *>(realloc(array->data, newCapacity * sizeof(double)));
        array->data = tmp; // copies the the memory address stored in tmp into data, updating the location
        array->capacity = newCapacity; // update capacity after reallocation
    }
 
    array->data[array->moveCount++] = xTarget; // store user input target into the array
    return 0;
}

void dispRecords(const TargetArray *array) {
    cout << "Here are the previous travel distance targets in meters: " << endl;
    for (int i = 0; i < array->moveCount; i++) {
        cout << array->data[i] << " meter(s)" << endl;
    }
    cout << "  " << endl;
}

void resetRecords(TargetArray *array) {
    free(array->data);
    targetArrayInit(array);
}

double currentPosition(TargetArray *array) {
    // sum all entries in the record
    double position = 0.0;

    for (int i = 0; i < array->moveCount; i++) {
        position += array->data[i];
    }
    return position;
}

double addTarget(TargetArray *array) {
    double target;
    cout << "Enter target distance to travel forward (+) or backwards (-) in meters: ";
    cin >> target; // take target distance of travel
    cout << " " << endl;

    if (appendRecord(array, target) != 0) { // return error if data is not successfully written
                return -1;
    }
    return 0;
}