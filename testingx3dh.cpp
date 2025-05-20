#include <sodium.h>
#include <iostream>
#include <iomanip>
#include <sstream>

constexpr size_t KEY_LEN = crypto_scalarmult_BYTES;

std::string bin2hex(const unsigned char* bin, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)bin[i];
    return oss.str();
}

void x3dh(
    const unsigned char* identity_key_public,
    const unsigned char* identity_key_private,
    const unsigned char* ephemeral_key_public,
    const unsigned char* ephemeral_key_private,
    const unsigned char* signed_prekey_public,
    const unsigned char* signed_prekey_private,
    const unsigned char* one_time_prekey_public,
    const unsigned char* one_time_prekey_private,
    bool is_initiator) {
    
    unsigned char dh1[KEY_LEN], dh2[KEY_LEN], dh3[KEY_LEN], dh4[KEY_LEN];
    
    if (is_initiator) {
        if (crypto_scalarmult(dh1, identity_key_private, signed_prekey_public) != 0)
            throw std::runtime_error("DH1 failed");
            
        if (crypto_scalarmult(dh2, ephemeral_key_private, identity_key_public) != 0)
            throw std::runtime_error("DH2 failed");
            
        if (crypto_scalarmult(dh3, ephemeral_key_private, signed_prekey_public) != 0)
            throw std::runtime_error("DH3 failed");
            
        if (crypto_scalarmult(dh4, ephemeral_key_private, one_time_prekey_public) != 0)
            throw std::runtime_error("DH4 failed");
    } else {
        if (crypto_scalarmult(dh1, signed_prekey_private, identity_key_public) != 0)
            throw std::runtime_error("DH1 failed");
            
        if (crypto_scalarmult(dh2, identity_key_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH2 failed");
            
        if (crypto_scalarmult(dh3, signed_prekey_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH3 failed");
            
        if (crypto_scalarmult(dh4, one_time_prekey_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH4 failed");
    }
    
    unsigned char ikm[KEY_LEN * 4];
    memcpy(ikm, dh1, KEY_LEN);
    memcpy(ikm + KEY_LEN, dh2, KEY_LEN);
    memcpy(ikm + 2 * KEY_LEN, dh3, KEY_LEN);
    memcpy(ikm + 3 * KEY_LEN, dh4, KEY_LEN);
    
    unsigned char shared_secret[KEY_LEN];
    crypto_generichash(shared_secret, KEY_LEN, ikm, sizeof(ikm), nullptr, 0);
    
    std::cout << (is_initiator ? "INITIATOR" : "RESPONDER") << " SHARED SECRETS:" << std::endl;
    std::cout << "DH1: " << bin2hex(dh1, KEY_LEN) << std::endl;
    std::cout << "DH2: " << bin2hex(dh2, KEY_LEN) << std::endl;
    std::cout << "DH3: " << bin2hex(dh3, KEY_LEN) << std::endl;
    std::cout << "DH4: " << bin2hex(dh4, KEY_LEN) << std::endl;
    
    std::cout << "\nFinal X3DH Shared Secret (Root Key): " << bin2hex(shared_secret, KEY_LEN) << std::endl;
}

int main(int argc, char *argv[]) {
    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return 1;
    }
    
    std::cout << "Generating keys for party A..." << std::endl;
    unsigned char a_public_longterm_key[crypto_box_PUBLICKEYBYTES];
    unsigned char a_private_longterm_key[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(a_public_longterm_key, a_private_longterm_key);
    
    unsigned char a_public_signed_key[crypto_box_PUBLICKEYBYTES];
    unsigned char a_private_signed_key[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(a_public_signed_key, a_private_signed_key);
    
    unsigned char a_public_onetime_key[crypto_box_PUBLICKEYBYTES];
    unsigned char a_private_onetime_key[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(a_public_onetime_key, a_private_onetime_key);
    
    std::cout << "Generating keys for party B..." << std::endl;
    unsigned char b_public_longterm_key[crypto_box_PUBLICKEYBYTES];
    unsigned char b_private_longterm_key[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(b_public_longterm_key, b_private_longterm_key);
    
    unsigned char b_public_signed_key[crypto_box_PUBLICKEYBYTES];
    unsigned char b_private_signed_key[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(b_public_signed_key, b_private_signed_key);
    
    unsigned char b_public_onetime_key[crypto_box_PUBLICKEYBYTES];
    unsigned char b_private_onetime_key[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(b_public_onetime_key, b_private_onetime_key);
    
    std::cout << "Party A longterm public key: " << bin2hex(a_public_longterm_key, crypto_box_PUBLICKEYBYTES) << std::endl;
    std::cout << "Party A signed public key: " << bin2hex(a_public_signed_key, crypto_box_PUBLICKEYBYTES) << std::endl;
    std::cout << "Party A onetime public key: " << bin2hex(a_public_onetime_key, crypto_box_PUBLICKEYBYTES) << std::endl;
    
    std::cout << "Party B longterm public key: " << bin2hex(b_public_longterm_key, crypto_box_PUBLICKEYBYTES) << std::endl;
    std::cout << "Party B signed public key: " << bin2hex(b_public_signed_key, crypto_box_PUBLICKEYBYTES) << std::endl;
    std::cout << "Party B onetime public key: " << bin2hex(b_public_onetime_key, crypto_box_PUBLICKEYBYTES) << std::endl;

    std::cout << "\nPerforming X3DH key agreement from both perspectives..." << std::endl;
    
    x3dh(
        b_public_longterm_key,
        a_private_longterm_key,
        a_public_onetime_key,
        a_private_onetime_key,
        b_public_signed_key,
        nullptr,
        b_public_onetime_key,
        nullptr,
        true
    );
    
    x3dh(
        a_public_longterm_key,
        b_private_longterm_key,
        a_public_onetime_key,
        nullptr,
        b_public_signed_key,
        b_private_signed_key,
        b_public_onetime_key,
        b_private_onetime_key,
        false
    );
         
    return 0;
}