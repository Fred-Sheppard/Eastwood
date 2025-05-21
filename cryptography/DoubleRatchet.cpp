#include "DoubleRatchet.h"
#include <cstring>
#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>

// Context strings for key derivation - must be exactly 8 bytes
const char* const ROOT_CTX = "DRROOT01";
const char* const CHAIN_CTX = "DRCHAIN1";
const char* const MSG_CTX = "DRMSG001";

DoubleRatchet::DoubleRatchet(const unsigned char* x3dh_root_key,
              const unsigned char* remote_public_signed_prekey,
              const unsigned char* local_public_ephemeral,
              const unsigned char* local_private_ephemeral) {
    // Initialize key arrays with zeros
    memset(send_key, 0, crypto_kdf_KEYBYTES);
    memset(recv_key, 0, crypto_kdf_KEYBYTES);
    
    memcpy(local_dh_public, local_public_ephemeral, crypto_box_PUBLICKEYBYTES);
    memcpy(local_dh_private, local_private_ephemeral, crypto_box_PUBLICKEYBYTES);
    memcpy(root_key, x3dh_root_key, crypto_box_PUBLICKEYBYTES);
    memcpy(remote_dh_public, remote_public_signed_prekey, crypto_box_PUBLICKEYBYTES);
    
    std::cout << "DoubleRatchet initialized with root key: ";
    for (int i = 0; i < crypto_kdf_KEYBYTES; i++)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)root_key[i];
    std::cout << std::endl;
}

unsigned char* DoubleRatchet::message_send() {
    // Generate new DH keypair
    crypto_kx_keypair(local_dh_public, local_dh_private);

    unsigned char shared_secret[crypto_scalarmult_BYTES];
    if (crypto_scalarmult(shared_secret, local_dh_private, remote_dh_public) != 0) {
        throw std::runtime_error("Error in crypto_scalarmult");
    }

    // Combine root_key and shared_secret
    unsigned char combined[64];
    memcpy(combined, root_key, crypto_kdf_KEYBYTES);
    memcpy(combined + crypto_kdf_KEYBYTES, shared_secret, crypto_scalarmult_BYTES);

    unsigned char master_key[crypto_kdf_KEYBYTES];

    // Hash combined keys to get new master key
    crypto_generichash(master_key, crypto_kdf_KEYBYTES, combined, sizeof combined, NULL, 0);

    // Derive new root_key using ROOT context
    if (crypto_kdf_derive_from_key(root_key, crypto_kdf_KEYBYTES, 1, ROOT_CTX, master_key) != 0) {
        throw std::runtime_error("Failed to derive new root key");
    }

    // Derive send chain key using CHAIN context
    if (crypto_kdf_derive_from_key(send_key, crypto_kdf_KEYBYTES, 2, CHAIN_CTX, master_key) != 0) {
        throw std::runtime_error("Failed to derive send chain key");
    }

    // Derive message key from chain key
    unsigned char* message_key = new unsigned char[crypto_kdf_KEYBYTES];
    if (crypto_kdf_derive_from_key(message_key, crypto_kdf_KEYBYTES, 1, MSG_CTX, send_key) != 0) {
        throw std::runtime_error("Failed to derive message key");
    }

    // Update chain key 
    if (crypto_kdf_derive_from_key(send_key, crypto_kdf_KEYBYTES, 2, CHAIN_CTX, send_key) != 0) {
        throw std::runtime_error("Failed to update chain key");
    }

    return message_key;
}

unsigned char* DoubleRatchet::message_receive(const unsigned char* new_remote_public_key) {
    // Update remote public key from parameter
    memcpy(remote_dh_public, new_remote_public_key, crypto_kx_PUBLICKEYBYTES);

    // Compute shared secret
    unsigned char shared_secret[crypto_scalarmult_BYTES];
    if (crypto_scalarmult(shared_secret, local_dh_private, remote_dh_public) != 0) {
        throw std::runtime_error("Error in crypto_scalarmult");
    }

    // Combine root_key and shared_secret
    unsigned char combined[64];
    memcpy(combined, root_key, crypto_kdf_KEYBYTES);
    memcpy(combined + crypto_kdf_KEYBYTES, shared_secret, crypto_scalarmult_BYTES);

    unsigned char master_key[crypto_kdf_KEYBYTES];

    // Hash combined keys to get new master key
    crypto_generichash(master_key, crypto_kdf_KEYBYTES, combined, sizeof combined, NULL, 0);

    // Derive new root_key using ROOT context
    if (crypto_kdf_derive_from_key(root_key, crypto_kdf_KEYBYTES, 1, ROOT_CTX, master_key) != 0) {
        throw std::runtime_error("Failed to derive new root key");
    }

    // Derive recv chain key using CHAIN context (same context as send chain)
    if (crypto_kdf_derive_from_key(recv_key, crypto_kdf_KEYBYTES, 2, CHAIN_CTX, master_key) != 0) {
        throw std::runtime_error("Failed to derive recv chain key");
    }

    // Derive message key from chain key
    unsigned char* message_key = new unsigned char[crypto_kdf_KEYBYTES];
    if (crypto_kdf_derive_from_key(message_key, crypto_kdf_KEYBYTES, 1, MSG_CTX, recv_key) != 0) {
        throw std::runtime_error("Failed to derive message key");
    }

    // Update chain key
    if (crypto_kdf_derive_from_key(recv_key, crypto_kdf_KEYBYTES, 2, CHAIN_CTX, recv_key) != 0) {
        throw std::runtime_error("Failed to update chain key");
    }

    return message_key;
}

const unsigned char* DoubleRatchet::get_public_key() const {
    return local_dh_public;
}

void DoubleRatchet::print_state() const {
    std::cout << "Root key: ";
    for (int i = 0; i < crypto_kdf_KEYBYTES; i++)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)root_key[i];
    std::cout << std::endl;
    
    std::cout << "Send key: ";
    for (int i = 0; i < crypto_kdf_KEYBYTES; i++)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)send_key[i];
    std::cout << std::endl;
    
    std::cout << "Recv key: ";
    for (int i = 0; i < crypto_kdf_KEYBYTES; i++)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)recv_key[i];
    std::cout << std::endl;
}
