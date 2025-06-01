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
    auto ratchet_id_arr = bundle->get_ratchet_id();

    bool exists = ratchets.find(ratchet_id_arr) != ratchets.end();
    std::cout << "Ratchets exists: " << exists << std::endl;

    std::vector<std::tuple<IdentitySessionId, SendingKeyBundle*>> posts_to_send;

    if (!exists) {
        // Create new DoubleRatchet instance with the bundle
        std::cout << "Creating ratchet" << std::endl;
        auto ratchet = std::make_unique<NewRatchet>(bundle->create_ratchet());
        ratchets[ratchet_id_arr] = std::move(ratchet);

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

    auto my_device_key = get_public_key("device");

    for (const auto& [id, ratchet]: ratchets) {
        auto [key, message_header] = ratchet->advance_send();
        
        // Set the device_id in the header now that we have proper database context
        memcpy(message_header->device_id, my_device_key.data(), crypto_box_PUBLICKEYBYTES);
        
        // Debug encryption
        std::cout << "Encryption key (first 4 bytes): ";
        for (int i = 0; i < 4; i++) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)key[i] << " ";
        }
        std::cout << std::endl;
        
        std::cout << "Original message length: " << message_len << std::endl;
        std::cout << "Original message (first 4 bytes): ";
        for (int i = 0; i < 4 && i < message_len; i++) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)message[i] << " ";
        }
        std::cout << std::endl;
        
        // create header
        std::unique_ptr<MessageHeader> header(message_header);

        // encrypt
        auto encrypted = encrypt_message_given_key(message, message_len, key);
        
        std::cout << "Encrypted length: " << encrypted.size() << std::endl;
        std::cout << "Encrypted (first 4 bytes): ";
        for (int i = 0; i < 4 && i < encrypted.size(); i++) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)encrypted[i] << " ";
        }
        std::cout << std::endl;

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

unsigned char* IdentitySession::receive_message(const DeviceMessage& message) {
    size_t message_len = 32;
    
    auto my_device_key = get_public_key("device");
    
    auto ratchet_id_ptr = concat_ordered(reinterpret_cast<const unsigned char *>(my_device_key.data()), 32, message.header->device_id, 32, message_len);
    std::array<unsigned char, 32> ratchet_id_arr{};
    crypto_generichash(ratchet_id_arr.data(), 32, ratchet_id_ptr, message_len, nullptr, 0);
    
    auto ratchet_it = ratchets.find(ratchet_id_arr);
    if (ratchet_it == ratchets.end()) {
        throw std::runtime_error("Ratchet not found for the given ID");
    }
    
    auto* ratchet_ptr = ratchet_it->second.get();
    if (!ratchet_ptr) {
        throw std::runtime_error("Ratchet pointer is null");
    }

    auto key = ratchet_ptr->advance_receive(message.header);

    // Debug: Check the decryption inputs
    std::cout << "Decryption key (first 4 bytes): ";
    for (int i = 0; i < 4; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)key[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Ciphertext length: " << message.length << std::endl;
    std::cout << "Ciphertext (first 4 bytes): ";
    for (int i = 0; i < 4 && i < message.length; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)message.ciphertext[i] << " ";
    }
    std::cout << std::endl;
    
    return key;
}


