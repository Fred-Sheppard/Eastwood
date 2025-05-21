#ifndef DOUBLERATCHET_H
#define DOUBLERATCHET_H

#include <cstring>
#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include <string>

std::string bin2hex(const unsigned char* bin, size_t len);

struct Chain{
    unsigned char chain_key[crypto_kdf_KEYBYTES];
    int index;
};

class DoubleRatchet {
public:
    DoubleRatchet(const unsigned char* x3dh_root_key,
                  const unsigned char* remote_public_signed_prekey,
                  const unsigned char* local_public_ephemeral,
                  const unsigned char* local_private_ephemeral);

    unsigned char* message_send();

    unsigned char* message_receive(const unsigned char* new_remote_public_key);

    const unsigned char* get_public_key() const;

    void print_state() const;

private:
    // Performs a Diffie-Hellman ratchet step and updates the root key and chain key
    void dh_ratchet(const unsigned char* remote_public_key, bool is_sending);
    
    // Derive a new set of keys (root key and chain key) from DH output
    void kdf_ratchet(const unsigned char* shared_secret, unsigned char* chain_key, bool is_sending);
    
    // Derive a message key from a chain key and updates the chain key
    unsigned char* derive_message_key(unsigned char* chain_key);

    unsigned char root_key[crypto_kdf_KEYBYTES];

    Chain send_chain;
    Chain recv_chain;

    unsigned char local_dh_public[crypto_kx_PUBLICKEYBYTES];
    unsigned char local_dh_private[crypto_kx_SECRETKEYBYTES];

    unsigned char remote_dh_public[crypto_kx_PUBLICKEYBYTES];
};

#endif //DOUBLERATCHET_H
