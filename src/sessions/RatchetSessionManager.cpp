//
// Created by Josh Sloggett on 01/06/2025.
//

#include "RatchetSessionManager.h"

RatchetSessionManager::RatchetSessionManager() {
    ratchets = {};
}

void RatchetSessionManager::create_ratchets_if_needed(std::string username, std::vector<KeyBundle*> bundles) {
    auto& user_ratchets = ratchets[username];

    for (KeyBundle* bundle : bundles) {
        std::array<unsigned char, 32> device_id;
        std::memcpy(device_id.data(), bundle->get_their_device_public(), 32);

        if (user_ratchets.find(device_id) == user_ratchets.end()) {
            user_ratchets[device_id] = bundle->create_ratchet();
        }
    }
}


std::map<std::array<unsigned char, 32>, std::tuple<std::array<unsigned char, 32>, MessageHeader *> > RatchetSessionManager::get_keys_for_identity(std::string username) {
    std::map<std::array<unsigned char, 32>, std::tuple<std::array<unsigned char, 32>, MessageHeader *> > keys;
    auto ratchets_for_user = ratchets[username];

    for (auto [device_id, ratchet] : ratchets_for_user) {
        keys[device_id] = ratchet->advance_send();
    }

    return keys;
}


unsigned char* RatchetSessionManager::get_key_for_device(std::string username, MessageHeader* header) {
    std::array<unsigned char, 32> device_id;
    std::copy(std::begin(header->device_id), std::end(header->device_id), device_id.begin());
    auto target_ratchet = ratchets[username].find(device_id);
    return target_ratchet->second->advance_receive(header);
}

