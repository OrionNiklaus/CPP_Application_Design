#pragma once
#include <cstddef> // for size_t
using namespace std;

// create dynamic array for positions
struct TargetArray {
    double *data; // heap block to hold travel target values
    size_t moveCount; // number of stored travel target values in data array
    size_t capacity; // how many slots allocated in data array
};

// function to initialize target record array
void targetArrayInit(TargetArray *array);

// function to append a target to record array
double appendRecord(TargetArray *array, double xTarget);

// display all target records
void dispRecords(const TargetArray *array);

// reset all records and release memory
void resetRecords(TargetArray *array);

// calculate current position w.r.t initial position
double currentPosition(TargetArray *array);

// takes user input, target, and appends to the record
double addTarget(TargetArray *array);