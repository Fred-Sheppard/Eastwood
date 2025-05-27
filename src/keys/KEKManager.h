#pragma once
#include <memory>
#include "SecureMemoryBuffer.h"

class KEKManager {
public:
    static KEKManager& instance();

    void setKEK(std::unique_ptr<SecureMemoryBuffer> kek);
    SecureMemoryBuffer* getKEK() const;
    bool isLoaded() const;

    // Non-copyable
    KEKManager(const KEKManager&) = delete;
    KEKManager& operator=(const KEKManager&) = delete;
private:
    KEKManager() = default;
    std::unique_ptr<SecureMemoryBuffer> kek_;
}; 