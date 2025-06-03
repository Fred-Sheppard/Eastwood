//
// Created by Josh Sloggett on 31/05/2025.
//

#pragma once

#include "sodium.h"
#include <cstring>
#include <array>
#include <string>

struct MessageHeader {
    // Constructor to ensure proper initialization
    MessageHeader() : dh_public{}, prev_chain_length(0), message_index(0), device_id{} {
        memset(file_uuid, 0, sizeof(file_uuid));
    }
    
    std::array<unsigned char, crypto_kx_PUBLICKEYBYTES> dh_public; // Sender's current ratchet public key
    int prev_chain_length;                                         // Length of previous sending chain
    int message_index;                                             // Message number in the chain
    std::array<unsigned char, crypto_box_PUBLICKEYBYTES> device_id; // Fixed-size array for device ID
    char file_uuid[64]; // Fixed-size array instead of std::string
};

struct Message {
    MessageHeader header;
    unsigned char* message;
};

// Common message class for device communication
class DeviceMessage {
public:
    DeviceMessage() : header{}, ciphertext(nullptr), length(0) {}

    ~DeviceMessage() {
        if (ciphertext) {
            delete[] ciphertext;
            ciphertext = nullptr;
        }
    }

    // Copy constructor
    DeviceMessage(const DeviceMessage& other) : header(other.header), length(other.length) {
        if (other.ciphertext && other.length > 0) {
            ciphertext = new unsigned char[other.length];
            memcpy(ciphertext, other.ciphertext, other.length);
        } else {
            ciphertext = nullptr;
        }
    }

    // Move constructor
    DeviceMessage(DeviceMessage&& other) noexcept
        : header(std::move(other.header)), ciphertext(other.ciphertext), length(other.length) {
        other.ciphertext = nullptr;
        other.length = 0;
    }

    // Assignment operator
    DeviceMessage& operator=(const DeviceMessage& other) {
        if (this != &other) {
            // Clean up existing ciphertext
            if (ciphertext) {
                delete[] ciphertext;
                ciphertext = nullptr;
            }

            // Copy header directly
            header = other.header;

            // Copy ciphertext
            if (other.ciphertext && other.length > 0) {
                ciphertext = new unsigned char[other.length];
                memcpy(ciphertext, other.ciphertext, other.length);
            } else {
                ciphertext = nullptr;
            }

            length = other.length;
        }
        return *this;
    }

    // Move assignment operator
    DeviceMessage& operator=(DeviceMessage&& other) noexcept {
        if (this != &other) {
            // Clean up existing ciphertext
            if (ciphertext) {
                delete[] ciphertext;
            }

            header = std::move(other.header);
            ciphertext = other.ciphertext;
            length = other.length;

            other.ciphertext = nullptr;
            other.length = 0;
        }
        return *this;
    }

    MessageHeader header;
    unsigned char* ciphertext;
    size_t length;
};

