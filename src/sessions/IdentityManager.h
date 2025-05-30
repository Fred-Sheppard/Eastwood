//
// Created by Josh Sloggett on 28/05/2025.
//

#ifndef EASTWOOD_IDENTITY_MANAGER_H
#define EASTWOOD_IDENTITY_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "IdentitySession.h"

// Custom comparator for unsigned char* keys
struct IdentityIdComparator {
    bool operator()(const unsigned char* a, const unsigned char* b) const {
        return memcmp(a, b, crypto_hash_sha256_BYTES) < 0;
    }
};

class IdentityManager {
private:
    std::map<unsigned char*, std::unique_ptr<IdentitySession>, IdentityIdComparator> _sessions;
    static IdentityManager* _instance;

public:
    static IdentityManager& getInstance() {
        static IdentityManager instance;
        return instance;
    }
    void update_or_create_identity_sessions(std::vector<KeyBundle*> bundles, std::string username_one, std::string username_two);
    void update_or_create_identity_sessions(std::vector<KeyBundle*> bundles, unsigned char* identity_session_id);
    void send_to_user(std::string username, unsigned char* msg);
};

#endif //EASTWOOD_IDENTITY_MANAGER_H
