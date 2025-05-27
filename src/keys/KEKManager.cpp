#include "KEKManager.h"
#include <stdexcept>

KEKManager& KEKManager::instance() {
    static KEKManager instance;
    return instance;
}

void KEKManager::setKEK(std::unique_ptr<SecureMemoryBuffer> kek) {
    kek_ = std::move(kek);
}

SecureMemoryBuffer* KEKManager::getKEK() const {
    if (!kek_) throw std::runtime_error("KEK not loaded");
    return kek_.get();
}

bool KEKManager::isLoaded() const {
    return kek_ != nullptr;
} 