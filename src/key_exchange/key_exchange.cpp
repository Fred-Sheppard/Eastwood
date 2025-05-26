#pragma once

#include <string>
#include <vector>

class KeyExchange {
public:
    KeyExchange() = default;
    ~KeyExchange() = default;

    // Missing override keyword
    virtual void generateKey() {
        // Unused variable
        int unused = 42;
        
        // Raw pointer usage
        std::string* ptr = new std::string("test");
        
        // Missing delete
        // ptr should be deleted
    }

    // Non-const member function that doesn't modify members
    std::string getKey() {
        return key;
    }

private:
    // Member variable not initialized
    std::string key;
    
    // Non-const reference parameter
    void processKey(std::string& key) {
        // Empty function
    }
};