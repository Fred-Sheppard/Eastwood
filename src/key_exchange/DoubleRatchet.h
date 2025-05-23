#ifndef DOUBLERATCHET_H
#define DOUBLERATCHET_H

#include <cstring>
#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>

struct Chain {
    unsigned char chain_key[crypto_kdf_KEYBYTES];
    int index;
};

// Message header structure as per Double Ratchet specification
struct MessageHeader {
    unsigned char dh_public[crypto_kx_PUBLICKEYBYTES]; // Sender's current ratchet public key
    int prev_chain_length;                             // Length of previous sending chain
    int message_index;                                 // Message number in the chain
};

struct Message {
    MessageHeader *header;
    unsigned char* message;
};

// Structure to identify a skipped message
struct SkippedMessageKey {
    unsigned char dh_public[crypto_kx_PUBLICKEYBYTES];
    int message_index;
    
    // Comparison operator for map
    bool operator<(const SkippedMessageKey& other) const {
        int cmp = memcmp(dh_public, other.dh_public, crypto_kx_PUBLICKEYBYTES);
        if (cmp != 0) return cmp < 0;
        return message_index < other.message_index;
    }
};

// Common message class for device communication
class DeviceMessage {
public:
    DeviceMessage() : header(nullptr), ciphertext(nullptr), plaintext(nullptr), length(0) {}
    
    ~DeviceMessage() {
        if (header) delete header;
        if (ciphertext) delete[] ciphertext;
        if (plaintext) delete[] plaintext;
    }
    
    // Copy constructor
    DeviceMessage(const DeviceMessage& other) {
        if (other.header) {
            header = new MessageHeader();
            memcpy(header->dh_public, other.header->dh_public, crypto_kx_PUBLICKEYBYTES);
            header->prev_chain_length = other.header->prev_chain_length;
            header->message_index = other.header->message_index;
        } else {
            header = nullptr;
        }
        
        if (other.ciphertext) {
            ciphertext = new unsigned char[other.length];
            memcpy(ciphertext, other.ciphertext, other.length);
        } else {
            ciphertext = nullptr;
        }
        
        if (other.plaintext) {
            plaintext = new unsigned char[other.length];
            memcpy(plaintext, other.plaintext, other.length);
        } else {
            plaintext = nullptr;
        }
        
        length = other.length;
    }
    
    // Move constructor
    DeviceMessage(DeviceMessage&& other) noexcept 
        : header(other.header), ciphertext(other.ciphertext), 
          plaintext(other.plaintext), length(other.length) {
        other.header = nullptr;
        other.ciphertext = nullptr;
        other.plaintext = nullptr;
        other.length = 0;
    }
    
    // Assignment operator
    DeviceMessage& operator=(const DeviceMessage& other) {
        if (this != &other) {
            if (header) delete header;
            if (ciphertext) delete[] ciphertext;
            if (plaintext) delete[] plaintext;
            
            if (other.header) {
                header = new MessageHeader();
                memcpy(header->dh_public, other.header->dh_public, crypto_kx_PUBLICKEYBYTES);
                header->prev_chain_length = other.header->prev_chain_length;
                header->message_index = other.header->message_index;
            } else {
                header = nullptr;
            }
            
            if (other.ciphertext) {
                ciphertext = new unsigned char[other.length];
                memcpy(ciphertext, other.ciphertext, other.length);
            } else {
                ciphertext = nullptr;
            }
            
            if (other.plaintext) {
                plaintext = new unsigned char[other.length];
                memcpy(plaintext, other.plaintext, other.length);
            } else {
                plaintext = nullptr;
            }
            
            length = other.length;
        }
        return *this;
    }
    
    // Move assignment operator
    DeviceMessage& operator=(DeviceMessage&& other) noexcept {
        if (this != &other) {
            if (header) delete header;
            if (ciphertext) delete[] ciphertext;
            if (plaintext) delete[] plaintext;
            
            header = other.header;
            ciphertext = other.ciphertext;
            plaintext = other.plaintext;
            length = other.length;
            
            other.header = nullptr;
            other.ciphertext = nullptr;
            other.plaintext = nullptr;
            other.length = 0;
        }
        return *this;
    }
    
    MessageHeader* header;
    unsigned char* ciphertext;
    unsigned char* plaintext;
    size_t length;
};

class DoubleRatchet {
public:
    DoubleRatchet(const unsigned char* x3dh_root_key,
                  const unsigned char* remote_public_signed_prekey,
                  const unsigned char* local_public_ephemeral,
                  const unsigned char* local_private_ephemeral);
    
    ~DoubleRatchet();

    // Creates a message key and header for sending
    DeviceMessage message_send(unsigned char* message);

    // Processes a received message with header and returns the message key
    DeviceMessage message_receive(const DeviceMessage& encrypted_message);

    const unsigned char* get_public_key() const;

    void print_state() const;

private:
    // Performs a Diffie-Hellman ratchet step and updates the root key and chain key
    void dh_ratchet(const unsigned char* remote_public_key, bool is_sending);
    
    // Derive a new set of keys (root key and chain key) from DH output
    void kdf_ratchet(const unsigned char* shared_secret, unsigned char* chain_key, bool is_sending);
    
    // Derive a message key from a chain key and updates the chain key
    unsigned char* derive_message_key(unsigned char* chain_key);

    unsigned char root_key[crypto_kdf_KEYBYTES]{};

    Chain send_chain{};
    Chain recv_chain{};
    
    int prev_send_chain_length; // Length of previous sending chain

    unsigned char local_dh_public[crypto_kx_PUBLICKEYBYTES]{};
    unsigned char local_dh_private[crypto_kx_SECRETKEYBYTES]{};

    unsigned char remote_dh_public[crypto_kx_PUBLICKEYBYTES]{};
    
    // Cache of message keys for skipped/out-of-order messages
    std::map<SkippedMessageKey, unsigned char*> skipped_message_keys;
    
    // Maximum number of skipped message keys to keep in memory
    static const int MAX_SKIPPED_MESSAGE_KEYS = 100;
};

#endif //DOUBLERATCHET_H
