//
// Created by Josh Sloggett on 01/06/2025.
//

#include "RatchetSessionManager.h"
#include <iostream>

#include "src/endpoints/endpoints.h"
#include "src/utils/JsonParser.h"

// Singleton instance method
RatchetSessionManager& RatchetSessionManager::instance() {
    static RatchetSessionManager instance;
    return instance;
}

RatchetSessionManager::RatchetSessionManager() {
    // No need to initialize ratchets as it's already default-initialized
}

void RatchetSessionManager::create_ratchets_if_needed(std::string username, std::vector<KeyBundle*> bundles, bool post_to_server) {
    auto& user_ratchets = ratchets[username];

    for (KeyBundle* bundle : bundles) {
        std::array<unsigned char, 32> device_id;
        std::memcpy(device_id.data(), bundle->get_their_device_public(), 32);

        if (user_ratchets.find(device_id) == user_ratchets.end()) {
            std::cout << "creating ratchet for " << username << std::endl;
            user_ratchets[device_id] = bundle->create_ratchet();
            // Save the newly created ratchet
            user_ratchets[device_id]->save(username, device_id);

            auto sending_bundle = dynamic_cast<SendingKeyBundle*>(bundle);

            if (post_to_server && sending_bundle) {
                try {
                    post_handshake_device(
                        sending_bundle->get_their_device_public(),
                        sending_bundle->get_their_signed_public(),
                        sending_bundle->get_their_signed_signature(),
                        sending_bundle->get_their_onetime_public(),
                        sending_bundle->get_my_ephemeral_public()
                    );
                    std::cout << "Successfully posted handshake to server for " << username << std::endl;
                } catch (const webwood::HttpError& e) {
                    std::cerr << "Server error when posting handshake for " << username << ": " << e.what() << std::endl;
                    // Continue execution - ratchet is still created locally
                } catch (const std::exception& e) {
                    std::cerr << "Error posting handshake for " << username << ": " << e.what() << std::endl;
                    // Continue execution - ratchet is still created locally
                }
            }
        }
    }
    
    // Clean up KeyBundle objects after processing
    for (KeyBundle* bundle : bundles) {
        delete bundle;
    }
}


std::map<std::array<unsigned char, 32>, std::tuple<std::array<unsigned char, 32>, MessageHeader *> > RatchetSessionManager::get_keys_for_identity(std::string username, bool post_new_ratchets_to_server) {
    if (post_new_ratchets_to_server) {
        const auto new_bundles = get_keybundles(username, get_device_ids_of_existing_handshakes(username));
        create_ratchets_if_needed(username, new_bundles, post_new_ratchets_to_server);
    }

    std::map<std::array<unsigned char, 32>, std::tuple<std::array<unsigned char, 32>, MessageHeader *> > keys;
    auto& ratchets_for_user = ratchets[username];

    // Get my (sender's) device public key from database
    auto my_device_public = get_public_key("device");

    for (auto& [device_id, ratchet] : ratchets_for_user) {
        auto [message_key_vector, header] = ratchet->advance_send();
        ratchet->save(username, device_id);
        
        memcpy(header->device_id, my_device_public.constData(), 32);
        
        std::array<unsigned char, 32> message_key_array;
        std::copy(message_key_vector.begin(), message_key_vector.begin() + 32, message_key_array.begin());
        
        keys[device_id] = std::make_tuple(message_key_array, header);
    }

    return keys;
}


unsigned char* RatchetSessionManager::get_key_for_device(std::string username, MessageHeader* header) {
    std::array<unsigned char, 32> device_id;
    std::copy(std::begin(header->device_id), std::end(header->device_id), device_id.begin());
    
    auto user_it = ratchets.find(username);
    if (user_it == ratchets.end()) {
        throw std::runtime_error("User not found: " + username);
    }
    
    auto target_ratchet = user_it->second.find(device_id);
    if (target_ratchet == user_it->second.end()) {
        throw std::runtime_error("Device not found for user: " + username);
    }
    
    auto result = target_ratchet->second->advance_receive(header);
    
    target_ratchet->second->save(username, device_id);
    
    return result;
}

std::vector<std::array<unsigned char,32> > RatchetSessionManager::get_device_ids_of_existing_handshakes(std::string username) {
    std::vector<std::array<unsigned char,32> > device_ids;

    auto user_it = ratchets.find(username);
    if (user_it == ratchets.end()) {
        return device_ids;
    }

    for (const auto& [device_id, ratchet] : user_it->second) {
        device_ids.push_back(device_id);
    }
    return device_ids;
}

