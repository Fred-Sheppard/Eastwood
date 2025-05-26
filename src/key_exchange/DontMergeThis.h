#pragma once
#include "warning_example.h"

// Warning: using namespace in header
using namespace std;

// Warning: non-const global variable
int globalVar = 42;

// Warning: non-constexpr static member
static int staticVar = 100;

// Warning: implicit conversion
void WarningExample::processData(int& value) {
    // Warning: implicit conversion
    value = 3.14;
    
    // Warning: unused variable
    int unused = 42;
    
    // Warning: magic number
    if (value == 42) {
        // Warning: empty if body
    }
}

// Warning: unused parameter
void WarningExample::unusedParam(int x) {
    // Warning: implicit conversion
    x = 3.14;
}

// Warning: implicit conversion
void WarningExample::implicitConversion(long x) {
    // Warning: implicit conversion
    int y = x;
}

// Warning: raw pointer
int* WarningExample::getPointer() {
    // Warning: returning address of local variable
    int local = 42;
    return &local;
}

// Warning: non-virtual destructor
WarningExample::~WarningExample() {
    // Warning: empty destructor
}

// Warning: non-const static member
int WarningExample::globalCounter = 0;