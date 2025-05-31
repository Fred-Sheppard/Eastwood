//
// Created by Josh Sloggett on 28/05/2025.
//

#include "IdentitySession.h"
#include "src/endpoints/endpoints.h"
#include "src/key_exchange/utils.h"
#include "src/key_exchange/XChaCha20-Poly1305.h"

IdentitySession::IdentitySession(std::vector<KeyBundle*> const &keys, const unsigned char* identity_session_id_in) {
    std::cout << "IdentitySession::IdentitySession" << std::endl;
    memcpy(identity_session_id, identity_session_id_in, crypto_hash_sha256_BYTES);
    updateFromBundles(keys);
}

IdentitySession::~IdentitySession() {
    // Clean up any remaining resources
}

void IdentitySession::updateFromBundles(std::vector<KeyBundle*> bundles) {
    std::cout << "IdentitySession::updateFromBundles" << std::endl;
    std::cout << bundles.size() << std::endl;
    for (KeyBundle* bundle: bundles) {
        create_ratchet_if_needed(bundle);
    }
}

// returns handshakes to post if needed
// post_handshake(session_id, jadjfajdfja)
std::vector<std::tuple<IdentitySessionId, SendingKeyBundle*>> IdentitySession::create_ratchet_if_needed(KeyBundle* bundle) {
    size_t message_len = 32;
    auto ratchet_id = bundle->get_ratchet_id();

    bool exists = ratchets.find(ratchet_id) != ratchets.end();
    std::cout << "Ratchets exists: " << exists << std::endl;

    std::vector<std::tuple<IdentitySessionId, SendingKeyBundle*>> posts_to_send;

    if (!exists) {
        // Create new DoubleRatchet instance with the bundle
        std::cout << "Creating ratchet" << std::endl;
        auto ratchet = std::make_unique<NewRatchet>(bundle->create_ratchet());
        ratchets[ratchet_id] = std::move(ratchet);

        if (bundle->get_role() == Role::Initiator) {
            auto sender = dynamic_cast<SendingKeyBundle*>(bundle);
            IdentitySessionId session_id;
            memcpy(session_id.data.data(), identity_session_id, crypto_hash_sha256_BYTES);
            posts_to_send.push_back(std::make_tuple(session_id, sender));
        }
    }

    return posts_to_send;
}

std::vector<std::tuple<IdentitySessionId, std::unique_ptr<DeviceMessage>>> IdentitySession::send_message(const unsigned char* message, size_t message_len) {
    std::vector<std::tuple<IdentitySessionId, std::unique_ptr<DeviceMessage>>> responses;
    IdentitySessionId session_id{};
    memcpy(session_id.data.data(), identity_session_id, crypto_hash_sha256_BYTES);

    for (const auto& [id, ratchet]: ratchets) {
        auto [key, message_header] = ratchet->advance_send();
        
        // create header
        std::unique_ptr<MessageHeader> header(message_header);

        // encrypt
        auto encrypted = encrypt_message_given_key(message, message_len, key);
        
        // Create DeviceMessage
        auto device_message = std::make_unique<DeviceMessage>();
        device_message->header = header.release();  // Transfer ownership
        device_message->ciphertext = new unsigned char[encrypted.size()];
        device_message->length = encrypted.size();
        memcpy(device_message->ciphertext, encrypted.data(), encrypted.size());

        // add to responses
        responses.emplace_back(session_id, std::move(device_message));
    }
    return responses;
}

std::vector<unsigned char> IdentitySession::receive_message(DeviceMessage *message) {
    size_t message_len = 32;
    auto ratchet_id = concat_ordered(reinterpret_cast<const unsigned char *>(get_public_key("device").data()), 32, message->header->device_id, 32, message_len);

    auto key = ratchets[ratchet_id].get()->advance_receive(message->header);

    std::vector<unsigned char> plaintext = decrypt_message_given_key(message->ciphertext, message->length, key);
    return plaintext;
}


