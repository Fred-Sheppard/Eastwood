#pragma once

class WarningExample {
public:
    // Warning: non-virtual destructor
    ~WarningExample();
    
    // Warning: unused parameter
    void unusedParam(int x);
    
    // Warning: implicit conversion
    void implicitConversion(long x);
    
    // Warning: raw pointer
    int* getPointer();
    
    // Warning: implicit conversion
    void processData(int& value);
    
    // Warning: non-const static member
    static int globalCounter;
}; 