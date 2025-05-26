#include "warning_example.h"

// Warning: using namespace in implementation
using namespace std;

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
    
    // Warning: implicit conversion
    double d = value;
    
    // Warning: implicit conversion
    char c = value;
}

// Warning: unused parameter
void WarningExample::unusedParam(int x) {
    // Warning: implicit conversion
    x = 3.14;
    
    // Warning: unused variable
    int y = 10;
}

// Warning: implicit conversion
void WarningExample::implicitConversion(long x) {
    // Warning: implicit conversion
    int y = x;
    
    // Warning: implicit conversion
    char c = y;
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