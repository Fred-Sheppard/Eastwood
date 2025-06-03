#include "x3dh.h"
#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>

static std::string bin2hex(const unsigned char *bin, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bin[i]);
    return oss.str();
}

unsigned char *x3dh_initiator(
    std::unique_ptr<SecureMemoryBuffer> my_identity_key_private,
    std::shared_ptr<SecureMemoryBuffer> my_ephemeral_key_private,
    const unsigned char *recipient_identity_key_public,
    const unsigned char *recipient_signed_prekey_public,
    const unsigned char *recipient_onetime_prekey_public
) {
    std::cout << "\n===== INITIATOR X3DH =====" << std::endl;
    
    // Debug: Print all input keys with corresponding public keys
    std::cout << "INITIATOR KEYS:" << std::endl;
    std::cout << "  my_identity_private: " << bin2hex(my_identity_key_private->data(), 32) << std::endl;
    
    // Derive my identity public key from private
    unsigned char my_identity_public[32];
    crypto_scalarmult_base(my_identity_public, my_identity_key_private->data());
    std::cout << "  my_identity_public (derived): " << bin2hex(my_identity_public, 32) << std::endl;
    
    std::cout << "  my_ephemeral_private: " << bin2hex(my_ephemeral_key_private->data(), 32) << std::endl;
    
    // Derive my ephemeral public key from private
    unsigned char my_ephemeral_public[32];
    crypto_scalarmult_base(my_ephemeral_public, my_ephemeral_key_private->data());
    std::cout << "  my_ephemeral_public (derived): " << bin2hex(my_ephemeral_public, 32) << std::endl;
    
    std::cout << "  recipient_identity_public: " << bin2hex(recipient_identity_key_public, 32) << std::endl;
    std::cout << "  recipient_signed_prekey_public: " << bin2hex(recipient_signed_prekey_public, 32) << std::endl;
    if (recipient_onetime_prekey_public) {
        std::cout << "  recipient_onetime_prekey_public: " << bin2hex(recipient_onetime_prekey_public, 32) << std::endl;
    } else {
        std::cout << "  recipient_onetime_prekey_public: NULL" << std::endl;
    }

    constexpr size_t KEY_LEN = crypto_scalarmult_BYTES;

    unsigned char dh1[KEY_LEN], dh2[KEY_LEN], dh3[KEY_LEN], dh4[KEY_LEN];

    // DH1: my_identity_private * their_signed_prekey_public
    unsigned char my_id_x25519_sk[KEY_LEN];
    if (crypto_sign_ed25519_sk_to_curve25519(my_id_x25519_sk, my_identity_key_private->data()) != 0)
        throw std::runtime_error("Failed to convert my identity private key to X25519");
    std::cout << "  my_id_x25519_sk: " << bin2hex(my_id_x25519_sk, 32) << std::endl;
    
    if (crypto_scalarmult(dh1, my_id_x25519_sk, recipient_signed_prekey_public) != 0)
        throw std::runtime_error("DH1 failed");
    std::cout << "  DH1 result: " << bin2hex(dh1, 32) << std::endl;

    // DH2: my_ephemeral_private * their_identity_public
    unsigned char their_id_x25519_pk[KEY_LEN];
    if (crypto_sign_ed25519_pk_to_curve25519(their_id_x25519_pk, recipient_identity_key_public) != 0)
        throw std::runtime_error("Failed to convert recipient identity public key to X25519");
    std::cout << "  their_id_x25519_pk: " << bin2hex(their_id_x25519_pk, 32) << std::endl;
    
    if (crypto_scalarmult(dh2, my_ephemeral_key_private.get()->data(), their_id_x25519_pk) != 0)
        throw std::runtime_error("DH2 failed");
    std::cout << "  DH2 result: " << bin2hex(dh2, 32) << std::endl;

    // DH3: my_ephemeral_private * their_signed_prekey_public
    if (crypto_scalarmult(dh3, my_ephemeral_key_private.get()->data(), recipient_signed_prekey_public) != 0)
        throw std::runtime_error("DH3 failed");
    std::cout << "  DH3 result: " << bin2hex(dh3, 32) << std::endl;

    // DH4: my_ephemeral_private * their_onetime_prekey_public
    if (recipient_onetime_prekey_public && my_ephemeral_key_private.get()->data()) {
        if (crypto_scalarmult(dh4, my_ephemeral_key_private.get()->data(), recipient_onetime_prekey_public) != 0)
            throw std::runtime_error("DH4 failed");
        std::cout << "  DH4 result: " << bin2hex(dh4, 32) << std::endl;
    } else {
        memset(dh4, 0, KEY_LEN);
        std::cout << "  DH4 result: (ZEROED - no onetime key)" << std::endl;
    }

    unsigned char ikm[KEY_LEN * 4];
    memcpy(ikm, dh1, KEY_LEN);
    memcpy(ikm + KEY_LEN, dh2, KEY_LEN);
    memcpy(ikm + 2 * KEY_LEN, dh3, KEY_LEN);
    memcpy(ikm + 3 * KEY_LEN, dh4, KEY_LEN);

    std::cout << "  IKM: " << bin2hex(ikm, KEY_LEN * 4) << std::endl;

    unsigned char *shared_secret = new unsigned char[KEY_LEN];
    crypto_generichash(shared_secret, KEY_LEN, ikm, sizeof(ikm), nullptr, 0);

    std::cout << "\nFinal X3DH Shared Secret (Root Key): " << bin2hex(shared_secret, KEY_LEN) << std::endl;

    return shared_secret;
}

unsigned char *x3dh_responder(
    const unsigned char* initiator_identity_key_public,
    const unsigned char* initiator_ephemeral_key_public,
    std::unique_ptr<SecureMemoryBuffer> my_identity_key_private,
    std::unique_ptr<SecureMemoryBuffer> my_signed_prekey_private,
    std::unique_ptr<SecureMemoryBuffer> my_onetime_prekey_private
    ) {
    std::cout << "\n===== RESPONDER X3DH =====" << std::endl;

    // Debug: Print all input keys with corresponding public keys
    std::cout << "RESPONDER KEYS:" << std::endl;
    std::cout << "  initiator_identity_public: " << bin2hex(initiator_identity_key_public, 32) << std::endl;
    std::cout << "  initiator_ephemeral_public: " << bin2hex(initiator_ephemeral_key_public, 32) << std::endl;
    std::cout << "  my_identity_private: " << bin2hex(my_identity_key_private->data(), 32) << std::endl;
    
    // Derive my identity public key from private
    unsigned char my_identity_public[32];
    crypto_scalarmult_base(my_identity_public, my_identity_key_private->data());
    std::cout << "  my_identity_public (derived): " << bin2hex(my_identity_public, 32) << std::endl;
    
    std::cout << "  my_signed_prekey_private: " << bin2hex(my_signed_prekey_private->data(), 32) << std::endl;
    
    // Derive my signed prekey public from private
    unsigned char my_signed_prekey_public[32];
    crypto_scalarmult_base(my_signed_prekey_public, my_signed_prekey_private->data());
    std::cout << "  my_signed_prekey_public (derived): " << bin2hex(my_signed_prekey_public, 32) << std::endl;
    
    if (my_onetime_prekey_private) {
        std::cout << "  my_onetime_prekey_private: " << bin2hex(my_onetime_prekey_private->data(), 32) << std::endl;
        
        // Derive my onetime public key from private
        unsigned char my_onetime_prekey_public[32];
        crypto_scalarmult_base(my_onetime_prekey_public, my_onetime_prekey_private->data());
        std::cout << "  my_onetime_prekey_public (derived): " << bin2hex(my_onetime_prekey_public, 32) << std::endl;
    } else {
        std::cout << "  my_onetime_prekey_private: NULL" << std::endl;
    }

    constexpr size_t KEY_LEN = crypto_scalarmult_BYTES;

    unsigned char dh1[KEY_LEN], dh2[KEY_LEN], dh3[KEY_LEN], dh4[KEY_LEN];

    // DH1: my_signed_prekey_private * their_identity_public
    unsigned char their_id_x25519_pk[KEY_LEN];
    if (crypto_sign_ed25519_pk_to_curve25519(their_id_x25519_pk, initiator_identity_key_public) != 0)
        throw std::runtime_error("Failed to convert initiator identity public key to X25519");
    std::cout << "  their_id_x25519_pk: " << bin2hex(their_id_x25519_pk, 32) << std::endl;
    
    if (crypto_scalarmult(dh1, my_signed_prekey_private->data(), their_id_x25519_pk) != 0)
        throw std::runtime_error("DH1 failed");
    std::cout << "  DH1 result: " << bin2hex(dh1, 32) << std::endl;

    // DH2: my_identity_private * their_ephemeral_public
    unsigned char my_id_x25519_sk[KEY_LEN];
    if (crypto_sign_ed25519_sk_to_curve25519(my_id_x25519_sk, my_identity_key_private->data()) != 0)
        throw std::runtime_error("Failed to convert my identity private key to X25519");
    std::cout << "  my_id_x25519_sk: " << bin2hex(my_id_x25519_sk, 32) << std::endl;
    
    if (crypto_scalarmult(dh2, my_id_x25519_sk, initiator_ephemeral_key_public) != 0)
        throw std::runtime_error("DH2 failed");
    std::cout << "  DH2 result: " << bin2hex(dh2, 32) << std::endl;

    // DH3: my_signed_prekey_private * their_ephemeral_public
    if (crypto_scalarmult(dh3, my_signed_prekey_private->data(), initiator_ephemeral_key_public) != 0)
        throw std::runtime_error("DH3 failed");
    std::cout << "  DH3 result: " << bin2hex(dh3, 32) << std::endl;

    // DH4: my_onetime_prekey_private * their_ephemeral_public
    if (my_onetime_prekey_private && initiator_ephemeral_key_public) {
        if (crypto_scalarmult(dh4, my_onetime_prekey_private->data(), initiator_ephemeral_key_public) != 0)
            throw std::runtime_error("DH4 failed");
        std::cout << "  DH4 result: " << bin2hex(dh4, 32) << std::endl;
    } else {
        memset(dh4, 0, KEY_LEN);
        std::cout << "  DH4 result: (ZEROED - no onetime key)" << std::endl;
    }

    unsigned char ikm[KEY_LEN * 4];
    memcpy(ikm, dh1, KEY_LEN);
    memcpy(ikm + KEY_LEN, dh2, KEY_LEN);
    memcpy(ikm + 2 * KEY_LEN, dh3, KEY_LEN);
    memcpy(ikm + 3 * KEY_LEN, dh4, KEY_LEN);

    std::cout << "  IKM: " << bin2hex(ikm, KEY_LEN * 4) << std::endl;

    unsigned char *shared_secret = new unsigned char[KEY_LEN];
    crypto_generichash(shared_secret, KEY_LEN, ikm, sizeof(ikm), nullptr, 0);

    std::cout << "\nFinal X3DH Shared Secret (Root Key): " << bin2hex(shared_secret, KEY_LEN) << std::endl;

    return shared_secret;
}
