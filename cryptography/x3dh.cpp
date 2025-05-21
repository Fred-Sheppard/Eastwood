#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

static std::string bin2hex(const unsigned char* bin, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)bin[i];
    return oss.str();
}

unsigned char* x3dh(
    const unsigned char* identity_key_public,
    const unsigned char* identity_key_private,
    const unsigned char* ephemeral_key_public,
    const unsigned char* ephemeral_key_private,
    const unsigned char* signed_prekey_public,
    const unsigned char* signed_prekey_private,
    const unsigned char* one_time_prekey_public,
    const unsigned char* one_time_prekey_private,
    const unsigned char* signed_prekey_signature,
    const unsigned char* ed25519_identity_key_public,
    bool is_initiator) {
    
    if (is_initiator && signed_prekey_signature && ed25519_identity_key_public) {
        if (crypto_sign_verify_detached(
                signed_prekey_signature, 
                signed_prekey_public, 
                crypto_box_PUBLICKEYBYTES, 
                ed25519_identity_key_public) != 0) {
            throw std::runtime_error("Signature verification failed");
        }
        std::cout << "Signature verification successful" << std::endl;
    }
    
    constexpr size_t KEY_LEN = crypto_scalarmult_BYTES;
    
    unsigned char dh1[KEY_LEN], dh2[KEY_LEN], dh3[KEY_LEN], dh4[KEY_LEN];
    
    if (is_initiator) {
        // Initiator: My identity private key × Responder's signed prekey public
        if (crypto_scalarmult(dh1, identity_key_private, signed_prekey_public) != 0)
            throw std::runtime_error("DH1 failed");
    } else {
        // Responder: My signed prekey private × Initiator's identity key public
        if (crypto_scalarmult(dh1, signed_prekey_private, identity_key_public) != 0)
            throw std::runtime_error("DH1 failed");
    }
    
    if (is_initiator) {
        std::cout << "DH2 (Initiator): Using my ephemeral private key with responder's identity public key\n";
        
        if (crypto_scalarmult(dh2, ephemeral_key_private, identity_key_public) != 0)
            throw std::runtime_error("DH2 failed");
    } else {
        std::cout << "DH2 (Responder): Using my identity private key with initiator's ephemeral public key\n";
        if (crypto_scalarmult(dh2, identity_key_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH2 failed");
    }
    
    if (is_initiator) {
        // Initiator: My ephemeral private key × Responder's signed prekey public
        if (crypto_scalarmult(dh3, ephemeral_key_private, signed_prekey_public) != 0)
            throw std::runtime_error("DH3 failed");
    } else {
        // Responder: My signed prekey private × Initiator's ephemeral key public
        if (crypto_scalarmult(dh3, signed_prekey_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH3 failed");
    }
    
    if (is_initiator) {
        if (one_time_prekey_public && ephemeral_key_private) {
            // Initiator: My ephemeral private key × Responder's one-time prekey public
            if (crypto_scalarmult(dh4, ephemeral_key_private, one_time_prekey_public) != 0)
                throw std::runtime_error("DH4 failed");
        } else {
            memset(dh4, 0, KEY_LEN);
        }
    } else {
        if (one_time_prekey_private && ephemeral_key_public) {
            // Responder: My one-time prekey private × Initiator's ephemeral key public
            if (crypto_scalarmult(dh4, one_time_prekey_private, ephemeral_key_public) != 0)
                throw std::runtime_error("DH4 failed");
        } else {
            memset(dh4, 0, KEY_LEN);
        }
    }
    
    unsigned char ikm[KEY_LEN * 4];
    memcpy(ikm, dh1, KEY_LEN);
    memcpy(ikm + KEY_LEN, dh2, KEY_LEN);
    memcpy(ikm + 2 * KEY_LEN, dh3, KEY_LEN);
    memcpy(ikm + 3 * KEY_LEN, dh4, KEY_LEN);
    
    unsigned char* shared_secret = new unsigned char[KEY_LEN];
    crypto_generichash(shared_secret, KEY_LEN, ikm, sizeof(ikm), nullptr, 0);
    
    std::cout << (is_initiator ? "INITIATOR" : "RESPONDER") << " SHARED SECRETS:" << std::endl;
    std::cout << "DH1: " << bin2hex(dh1, KEY_LEN) << std::endl;
    std::cout << "DH2: " << bin2hex(dh2, KEY_LEN) << std::endl;
    std::cout << "DH3: " << bin2hex(dh3, KEY_LEN) << std::endl;
    std::cout << "DH4: " << bin2hex(dh4, KEY_LEN) << std::endl;
    
    std::cout << "\nFinal X3DH Shared Secret (Root Key): " << bin2hex(shared_secret, KEY_LEN) << std::endl;
    
    return shared_secret;
} 