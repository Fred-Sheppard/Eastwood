#include "DoubleRatchet.h"
#include "utils.h"
#include <cstring>
#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "XChaCha20-Poly1305.h"
#include "src/endpoints/endpoints.h"
#include "src/sessions/KeyBundle.h"

// Context strings for key derivation - must be exactly 8 bytes
const char* const ROOT_CTX = "DRROOT01";
const char* const CHAIN_CTX = "DRCHAIN1";
const char* const MSG_CTX = "DRMSG001";

DoubleRatchet::DoubleRatchet(KeyBundle* bundle) {
    send_chain.index = 0;
    recv_chain.index = 0;
    prev_send_chain_length = 0;

    // Initialize keys
    memset(send_chain.chain_key, 0, crypto_kdf_KEYBYTES);
    memset(recv_chain.chain_key, 0, crypto_kdf_KEYBYTES);

    // 1. Copy the X3DH shared secret as the initial root key
    if (bundle->get_role() == Role::Initiator) {
        auto* sender = dynamic_cast<SendingKeyBundle*>(bundle);
        if (!sender) throw std::runtime_error("Invalid bundle type for Initiator");
        memcpy(local_dh_public, sender->get_my_ephemeral_public(), crypto_box_PUBLICKEYBYTES);
        memcpy(local_dh_private, sender->get_my_ephemeral_private(), crypto_box_SECRETKEYBYTES);
        memcpy(remote_dh_public, sender->get_their_signed_public(), crypto_box_PUBLICKEYBYTES);
        memcpy(root_key, sender->get_shared_secret(), crypto_box_SECRETKEYBYTES);
    } else {
        auto* receiver = dynamic_cast<ReceivingKeyBundle*>(bundle);
        if (!receiver) throw std::runtime_error("Invalid bundle type for Responder");
        // For responder, use Bob's signed prekey private and Alice's ephemeral public
        memcpy(local_dh_private, receiver->get_my_signed_private(), crypto_box_SECRETKEYBYTES);
        memcpy(remote_dh_public, receiver->get_their_ephemeral_public(), crypto_box_PUBLICKEYBYTES);
        memcpy(root_key, receiver->get_shared_secret(), crypto_box_SECRETKEYBYTES);
    }

    // 2. Compute the initial DH output
    unsigned char dh_output[crypto_scalarmult_BYTES];
    if (crypto_scalarmult(dh_output, local_dh_private, remote_dh_public) != 0) {
        throw std::runtime_error("Failed to compute initial DH");
    }
    std::cout << "DH output: ";
    for (size_t i = 0; i < sizeof(dh_output); ++i) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)dh_output[i];
    std::cout << std::endl;

    // 3. Use a KDF to derive a new root key and a chain key from the previous root key and the DH output
    //    Both parties must use the same context and subkey indices
    unsigned char kdf_input[crypto_kdf_KEYBYTES + crypto_scalarmult_BYTES];
    memcpy(kdf_input, root_key, crypto_kdf_KEYBYTES);
    memcpy(kdf_input + crypto_kdf_KEYBYTES, dh_output, crypto_scalarmult_BYTES);
    std::cout << "KDF input: ";
    for (size_t i = 0; i < sizeof(kdf_input); ++i) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)kdf_input[i];
    std::cout << std::endl;

    unsigned char kdf_output[crypto_kdf_KEYBYTES * 2];
    crypto_generichash(kdf_output, sizeof(kdf_output), kdf_input, sizeof(kdf_input), nullptr, 0);

    // 4. Assign the derived keys
    memcpy(root_key, kdf_output, crypto_kdf_KEYBYTES);
    if (bundle->get_role() == Role::Initiator) {
        memcpy(send_chain.chain_key, kdf_output + crypto_kdf_KEYBYTES, crypto_kdf_KEYBYTES);
        std::cout << "Derived send_chain: ";
        for (size_t i = 0; i < crypto_kdf_KEYBYTES; ++i) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(kdf_output + crypto_kdf_KEYBYTES)[i];
        std::cout << std::endl;
    } else {
        memcpy(recv_chain.chain_key, kdf_output + crypto_kdf_KEYBYTES, crypto_kdf_KEYBYTES);
        std::cout << "Derived recv_chain: ";
        for (size_t i = 0; i < crypto_kdf_KEYBYTES; ++i) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(kdf_output + crypto_kdf_KEYBYTES)[i];
        std::cout << std::endl;
    }
    std::cout << "Derived root_key: ";
    for (size_t i = 0; i < crypto_kdf_KEYBYTES; ++i) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)root_key[i];
    std::cout << std::endl;

    std::cout << "DoubleRatchet initialized with root key: ";
    for (unsigned char i : root_key)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
    std::cout << std::endl;
}

DoubleRatchet::~DoubleRatchet() {
    // clean up any remaining cached message keys
    for (auto& pair : skipped_message_keys) {
        delete[] pair.second;
    }
    skipped_message_keys.clear();
}

void DoubleRatchet::dh_ratchet(const unsigned char* remote_public_key, bool is_sending) {
    if (is_sending) {
        // When sending, generate a new DH keypair
        crypto_kx_keypair(local_dh_public, local_dh_private);
        
        // Compute shared secret using DH
        unsigned char shared_secret[crypto_scalarmult_BYTES];
        if (crypto_scalarmult(shared_secret, local_dh_private, remote_public_key) != 0) {
            throw std::runtime_error("Error in crypto_scalarmult");
        }
        
        // Derive new keys from the shared secret
        unsigned char kdf_input[crypto_scalarmult_BYTES + crypto_kdf_KEYBYTES];
        memcpy(kdf_input, root_key, crypto_kdf_KEYBYTES);
        memcpy(kdf_input + crypto_kdf_KEYBYTES, shared_secret, crypto_scalarmult_BYTES);
        
        unsigned char kdf_output[2 * crypto_kdf_KEYBYTES];
        crypto_generichash(kdf_output, sizeof(kdf_output), kdf_input, sizeof(kdf_input), nullptr, 0);
        
        // Update root key and send chain key
        memcpy(root_key, kdf_output, crypto_kdf_KEYBYTES);
        memcpy(send_chain.chain_key, kdf_output + crypto_kdf_KEYBYTES, crypto_kdf_KEYBYTES);
        send_chain.index = 0;
    } else if (remote_public_key) {
        // When receiving, store the length of the current sending chain
        prev_send_chain_length = send_chain.index;
        
        // Update remote public key with the one from the message
        memcpy(remote_dh_public, remote_public_key, crypto_kx_PUBLICKEYBYTES);
        
        // Compute shared secret using DH
        unsigned char shared_secret[crypto_scalarmult_BYTES];
        if (crypto_scalarmult(shared_secret, local_dh_private, remote_public_key) != 0) {
            throw std::runtime_error("Error in crypto_scalarmult");
        }
        
        // Derive new keys from the shared secret
        unsigned char kdf_input[crypto_scalarmult_BYTES + crypto_kdf_KEYBYTES];
        memcpy(kdf_input, root_key, crypto_kdf_KEYBYTES);
        memcpy(kdf_input + crypto_kdf_KEYBYTES, shared_secret, crypto_scalarmult_BYTES);
        
        unsigned char kdf_output[2 * crypto_kdf_KEYBYTES];
        crypto_generichash(kdf_output, sizeof(kdf_output), kdf_input, sizeof(kdf_input), nullptr, 0);
        
        // Update root key and receive chain key
        memcpy(root_key, kdf_output, crypto_kdf_KEYBYTES);
        memcpy(recv_chain.chain_key, kdf_output + crypto_kdf_KEYBYTES, crypto_kdf_KEYBYTES);
        recv_chain.index = 0;
    }
}

void DoubleRatchet::kdf_ratchet(const unsigned char* shared_secret, unsigned char* chain_key, bool is_sending) {
    // Combine root_key and shared_secret
    unsigned char combined[64];
    memcpy(combined, root_key, crypto_kdf_KEYBYTES);
    memcpy(combined + crypto_kdf_KEYBYTES, shared_secret, crypto_scalarmult_BYTES);

    unsigned char master_key[crypto_kdf_KEYBYTES];

    // Hash combined keys to get new master key
    crypto_generichash(master_key, crypto_kdf_KEYBYTES, combined, sizeof combined, nullptr, 0);

    // Derive new root_key using ROOT context
    if (crypto_kdf_derive_from_key(root_key, crypto_kdf_KEYBYTES, 1, ROOT_CTX, master_key) != 0) {
        throw std::runtime_error("Failed to derive new root key");
    }

    // Derive chain key using CHAIN context
    if (crypto_kdf_derive_from_key(chain_key, crypto_kdf_KEYBYTES, 2, CHAIN_CTX, master_key) != 0) {
        throw std::runtime_error("Failed to derive chain key");
    }
}

unsigned char* DoubleRatchet::derive_message_key(unsigned char* chain_key) {
    // Derive message key from chain key
    auto* message_key = new unsigned char[crypto_kdf_KEYBYTES];
    if (crypto_kdf_derive_from_key(message_key, crypto_kdf_KEYBYTES, 0, MSG_CTX, chain_key) != 0) {
        throw std::runtime_error("Failed to derive message key");
    }
    
    return message_key;
}

void DoubleRatchet::advance_chain_key(unsigned char* chain_key) {
    // Update chain key for next message
    if (crypto_kdf_derive_from_key(chain_key, crypto_kdf_KEYBYTES, 1, CHAIN_CTX, chain_key) != 0) {
        throw std::runtime_error("Failed to update chain key");
    }
}

DeviceMessage DoubleRatchet::message_send(unsigned char* message, const unsigned char* device_id) {
    // Check if send chain is uninitialized (all zeros)
    bool send_chain_uninitialized = true;
    for (size_t i = 0; i < crypto_kdf_KEYBYTES; i++) {
        if (send_chain.chain_key[i] != 0) {
            send_chain_uninitialized = false;
            break;
        }
    }

    // If send chain is uninitialized or needs DH ratchet, perform it
    if (send_chain_uninitialized || needs_dh_ratchet_on_send) {
        dh_ratchet(remote_dh_public, true);
        needs_dh_ratchet_on_send = false;
    }

    DeviceMessage device_message;
    device_message.header = new MessageHeader{};

    // Populate header with current state
    memcpy(device_message.header->dh_public, local_dh_public, crypto_kx_PUBLICKEYBYTES);
    device_message.header->prev_chain_length = prev_send_chain_length;
    device_message.header->message_index = send_chain.index;
    memcpy(device_message.header->device_id, device_id, crypto_box_PUBLICKEYBYTES);

    // Derive message key for the current message
    unsigned char* message_key = derive_message_key(send_chain.chain_key);
    std::cout << "[DEBUG] Sender derived message key: " << bin2hex(message_key, crypto_kdf_KEYBYTES) << std::endl;
    std::cout.flush();

    // Increment the message index for the next message
    send_chain.index++;

    // Encrypt the message
    size_t message_len = strlen(reinterpret_cast<const char*>(message));
    std::vector<unsigned char> ciphertext_vec = encrypt_message_given_key(message, message_len, message_key);
    device_message.ciphertext = new unsigned char[ciphertext_vec.size()];
    memcpy(device_message.ciphertext, ciphertext_vec.data(), ciphertext_vec.size());
    device_message.length = ciphertext_vec.size();

    delete[] message_key;

    // Advance the chain key after encryption
    advance_chain_key(send_chain.chain_key);

    // Convert DeviceMessage to Message for post_ratchet_message
    post_ratchet_message(&device_message);
    return device_message;
}

std::vector<unsigned char> DoubleRatchet::message_receive(const DeviceMessage& message) {
    // Check if we need to perform a DH ratchet step
    if (memcmp(message.header->dh_public, remote_dh_public, crypto_kx_PUBLICKEYBYTES) != 0) {
        std::cout << "New DH ratchet key detected" << std::endl;
        // Cache skipped message keys from current chain up to the previous chain length
        std::cout << "Caching skipped message keys from current chain (" << recv_chain.index << " to " << message.header->prev_chain_length << ")" << std::endl;
        for (uint32_t i = recv_chain.index; i < message.header->prev_chain_length; i++) {
            unsigned char* message_key = derive_message_key(recv_chain.chain_key);
            SkippedMessageKey key_id = {{0}, static_cast<int>(i)};
            memcpy(key_id.dh_public, remote_dh_public, crypto_kx_PUBLICKEYBYTES);
            skipped_message_keys[key_id] = message_key;
            advance_chain_key(recv_chain.chain_key);
        }
        // Perform DH ratchet step for receiving chain
        dh_ratchet(message.header->dh_public, false);
        recv_chain.index = 0;
        memcpy(remote_dh_public, message.header->dh_public, crypto_kx_PUBLICKEYBYTES);
        needs_dh_ratchet_on_send = true;
        // Generate a new local DH keypair for the next sending chain
        crypto_kx_keypair(local_dh_public, local_dh_private);
        // Do NOT immediately perform a DH ratchet for the new sending chain; let it be initialized on the next DH ratchet event.
        // The send_chain will remain uninitialized (all zeros) until the next DH ratchet, as per the Double Ratchet spec.
        send_chain.index = 0;
    }
    // Skip ahead in receive chain if needed
    if (message.header->message_index > recv_chain.index) {
        std::cout << "Skipping ahead in receive chain from " << recv_chain.index << " to " << message.header->message_index << std::endl;
        for (uint32_t i = recv_chain.index; i < message.header->message_index; i++) {
            unsigned char* message_key = derive_message_key(recv_chain.chain_key);
            SkippedMessageKey key_id = {{0}, static_cast<int>(i)};
            memcpy(key_id.dh_public, message.header->dh_public, crypto_kx_PUBLICKEYBYTES);
            skipped_message_keys[key_id] = message_key;
            // Advance chain key
            advance_chain_key(recv_chain.chain_key);
        }
        recv_chain.index = message.header->message_index;
    }
    // Derive message key
    unsigned char* message_key = derive_message_key(recv_chain.chain_key);
    std::cout << "[DEBUG] Receiver derived message key: " << bin2hex(message_key, crypto_kdf_KEYBYTES) << std::endl;
    // Decrypt message
    std::vector<unsigned char> decrypted_message = decrypt_message_given_key(message.ciphertext, message.length, message_key);
    delete[] message_key;
    // Advance chain key
    advance_chain_key(recv_chain.chain_key);
    recv_chain.index++;
    return decrypted_message;
}

const unsigned char* DoubleRatchet::get_public_key() const {
    return local_dh_public;
}

void DoubleRatchet::print_state() const {
    std::cout << "Root key: " << bin2hex(root_key, crypto_kdf_KEYBYTES) << std::endl;
    std::cout << "Send chain (index " << send_chain.index << "): " 
              << bin2hex(send_chain.chain_key, crypto_kdf_KEYBYTES) << std::endl;
    std::cout << "Recv chain (index " << recv_chain.index << "): " 
              << bin2hex(recv_chain.chain_key, crypto_kdf_KEYBYTES) << std::endl;
    std::cout << "Previous send chain length: " << prev_send_chain_length << std::endl;
    std::cout << "Skipped message keys in cache: " << skipped_message_keys.size() << std::endl;
}
