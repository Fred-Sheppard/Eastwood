#include "utils.h"

unsigned char* convert_curve25519_to_ed25519(const unsigned char* curve25519_pubkey) {
    unsigned char* ed25519_pubkey = new unsigned char[crypto_sign_PUBLICKEYBYTES];
    
    // Convert Curve25519 public key to Ed25519 public key
    if (crypto_sign_ed25519_pk_to_curve25519(ed25519_pubkey, curve25519_pubkey) != 0) {
        delete[] ed25519_pubkey;
        throw std::runtime_error("Failed to convert Curve25519 public key to Ed25519");
    }
    
    return ed25519_pubkey;
}

unsigned char* concat_ordered(const unsigned char* arr1, size_t len1, const unsigned char* arr2, size_t len2, size_t total_len) {
    unsigned char* result = new unsigned char[total_len];
    memcpy(result, arr1, len1);
    memcpy(result + len1, arr2, len2);
    return result;
} 