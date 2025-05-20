//
// Created by Josh Sloggett on 20/05/2025.
//

#ifndef DOUBLERATCHET_H
#define DOUBLERATCHET_H

#include <cstring>
#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include <string>

// Utility function to convert binary data to hex string
std::string bin2hex(const unsigned char* bin, size_t len);

class DoubleRatchet {
public:
    /**
     * Initializes a Double Ratchet session
     * 
     * @param x3dh_root_key The shared secret from X3DH key agreement
     * @param remote_public_signed_prekey The remote party's signed prekey
     * @param local_public_ephemeral The local ephemeral public key 
     * @param local_private_ephemeral The local ephemeral private key
     */
    DoubleRatchet(const unsigned char* x3dh_root_key,
                  const unsigned char* remote_public_signed_prekey,
                  const unsigned char* local_public_ephemeral,
                  const unsigned char* local_private_ephemeral);

    /**
     * Prepares for sending a message by rotating DH keys and deriving new chain/message keys
     * 
     * @return The message key to use for encryption
     */
    unsigned char* message_send();

    /**
     * Updates the remote DH ratchet public key and processes ratchet steps
     * 
     * @param new_remote_public_key The new remote DH ratchet public key
     * @return The message key to use for decryption
     */
    unsigned char* message_receive(const unsigned char* new_remote_public_key);

    /**
     * Gets the current local DH ratchet public key
     * 
     * @return Pointer to the local DH ratchet public key
     */
    const unsigned char* get_public_key() const;

    /**
     * Prints the current state of the ratchet for debugging
     */
    void print_state() const;

private:
    unsigned char root_key[crypto_kdf_KEYBYTES];

    unsigned char send_key[crypto_kdf_KEYBYTES];
    unsigned char recv_key[crypto_kdf_KEYBYTES];

    unsigned char local_dh_public[crypto_kx_PUBLICKEYBYTES];
    unsigned char local_dh_private[crypto_kx_SECRETKEYBYTES];

    unsigned char remote_dh_public[crypto_kx_PUBLICKEYBYTES];
};

#endif //DOUBLERATCHET_H
