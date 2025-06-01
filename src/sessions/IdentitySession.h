//
// Created by Josh Sloggett on 28/05/2025.
//

#ifndef EASTWOOD_IDENTITY_SESSION_H
#define EASTWOOD_IDENTITY_SESSION_H

#include <vector>
#include <map>
#include <memory>
#include <array>
#include "IdentitySessionId.h"
#include "KeyBundle.h"
#include "src/key_exchange/NewRatchet.h"

class IdentitySession {
private:
    unsigned char identity_session_id[32];
    std::map<std::array<unsigned char, 32>, std::unique_ptr<NewRatchet>> ratchets;

public:
    IdentitySession(std::vector<KeyBundle*> const &keys, const unsigned char* identity_session_id_in);
    ~IdentitySession();

    void updateFromBundles(std::vector<KeyBundle*> bundles);
    std::vector<std::tuple<IdentitySessionId, SendingKeyBundle*>> create_ratchet_if_needed(KeyBundle* bundle);
    std::vector<std::tuple<IdentitySessionId, std::unique_ptr<DeviceMessage>>> send_message(const unsigned char* message, size_t message_len);
    unsigned char* receive_message(const DeviceMessage& message);

    const std::map<std::array<unsigned char, 32>, std::unique_ptr<NewRatchet>>& get_ratchets() const { return ratchets; };
};

#endif //EASTWOOD_IDENTITY_SESSION_H
