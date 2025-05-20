#include <iostream>
#include <iomanip>
#include <sstream>
#include <sodium.h>
#include "DoubleRatchet.h"

constexpr size_t KEY_LEN = crypto_scalarmult_BYTES;

std::string bin2hex(const unsigned char* bin, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)bin[i];
    return oss.str();
}

// X3DH key agreement
void x3dh(
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
    bool is_initiator,
    unsigned char* out_shared_secret) {
    
    if (is_initiator && signed_prekey_signature != nullptr && ed25519_identity_key_public != nullptr) {
        if (crypto_sign_verify_detached(
                signed_prekey_signature, 
                signed_prekey_public, 
                crypto_box_PUBLICKEYBYTES, 
                ed25519_identity_key_public) != 0) {
            throw std::runtime_error("Signature verification failed");
        }
        std::cout << "Signature verification successful" << std::endl;
    }
    
    unsigned char dh1[KEY_LEN], dh2[KEY_LEN], dh3[KEY_LEN], dh4[KEY_LEN];
    
    // DH1 = DH(IKA, SPKB) = DH(SPKB, IKA)
    if (is_initiator) {
        if (crypto_scalarmult(dh1, identity_key_private, signed_prekey_public) != 0)
            throw std::runtime_error("DH1 failed");
    } else {
        if (crypto_scalarmult(dh1, signed_prekey_private, identity_key_public) != 0)
            throw std::runtime_error("DH1 failed");
    }
    
    // DH2 = DH(EKA, IKB) = DH(IKB, EKA)
    if (is_initiator) {
        if (crypto_scalarmult(dh2, ephemeral_key_private, identity_key_public) != 0)
            throw std::runtime_error("DH2 failed");
    } else {
        if (crypto_scalarmult(dh2, identity_key_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH2 failed");
    }
    
    // DH3 = DH(EKA, SPKB) = DH(SPKB, EKA)
    if (is_initiator) {
        if (crypto_scalarmult(dh3, ephemeral_key_private, signed_prekey_public) != 0)
            throw std::runtime_error("DH3 failed");
    } else {
        if (crypto_scalarmult(dh3, signed_prekey_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH3 failed");
    }
    
    // DH4 = DH(EKA, OPKB) = DH(OPKB, EKA)
    if (is_initiator) {
        if (crypto_scalarmult(dh4, ephemeral_key_private, one_time_prekey_public) != 0)
            throw std::runtime_error("DH4 failed");
    } else {
        if (crypto_scalarmult(dh4, one_time_prekey_private, ephemeral_key_public) != 0)
            throw std::runtime_error("DH4 failed");
    }
    
    unsigned char ikm[KEY_LEN * 4];
    memcpy(ikm, dh1, KEY_LEN);
    memcpy(ikm + KEY_LEN, dh2, KEY_LEN);
    memcpy(ikm + 2 * KEY_LEN, dh3, KEY_LEN);
    memcpy(ikm + 3 * KEY_LEN, dh4, KEY_LEN);
    
    crypto_generichash(out_shared_secret, KEY_LEN, ikm, sizeof(ikm), nullptr, 0);
    
    std::cout << (is_initiator ? "INITIATOR" : "RESPONDER") << " SHARED SECRETS:" << std::endl;
    std::cout << "DH1: " << bin2hex(dh1, KEY_LEN) << std::endl;
    std::cout << "DH2: " << bin2hex(dh2, KEY_LEN) << std::endl;
    std::cout << "DH3: " << bin2hex(dh3, KEY_LEN) << std::endl;
    std::cout << "DH4: " << bin2hex(dh4, KEY_LEN) << std::endl;
    
    std::cout << "\nFinal X3DH Shared Secret (Root Key): " << bin2hex(out_shared_secret, KEY_LEN) << std::endl;
}

// Test Double Ratchet implementation
void test_double_ratchet() {
    std::cout << "\n===== TESTING DOUBLE RATCHET =====" << std::endl;
    
    // Initialize Alice and Bob's Double Ratchet with root key from X3DH
    unsigned char root_key[crypto_kdf_KEYBYTES];
    randombytes_buf(root_key, crypto_kdf_KEYBYTES);
    std::cout << "Initialized with root key: " << bin2hex(root_key, crypto_kdf_KEYBYTES) << std::endl;
    
    // Generate initial DH keypairs
    unsigned char alice_public[crypto_kx_PUBLICKEYBYTES];
    unsigned char alice_private[crypto_kx_SECRETKEYBYTES];
    crypto_kx_keypair(alice_public, alice_private);
    
    unsigned char bob_public[crypto_kx_PUBLICKEYBYTES];
    unsigned char bob_private[crypto_kx_SECRETKEYBYTES];
    crypto_kx_keypair(bob_public, bob_private);
    
    // Create Double Ratchet instances
    DoubleRatchet alice(root_key, bob_public, alice_public, alice_private);
    DoubleRatchet bob(root_key, alice_public, bob_public, bob_private);
    
    std::cout << "Alice's initial public key: " << bin2hex(alice_public, crypto_kx_PUBLICKEYBYTES) << std::endl;
    std::cout << "Bob's initial public key: " << bin2hex(bob_public, crypto_kx_PUBLICKEYBYTES) << std::endl;
    
    // Simulate several message exchanges
    for (int i = 0; i < 3; i++) {
        std::cout << "\n------ Message Exchange #" << (i+1) << " ------" << std::endl;
        
        // Alice sends to Bob
        std::cout << "Alice sending message:" << std::endl;
        unsigned char* alice_message_key = alice.message_send();
        std::cout << "  Alice's message key: " << bin2hex(alice_message_key, crypto_kdf_KEYBYTES) << std::endl;
        std::cout << "  Alice's new public key: " << bin2hex(alice.get_public_key(), crypto_kx_PUBLICKEYBYTES) << std::endl;
        
        // Bob receives from Alice
        std::cout << "Bob receiving message:" << std::endl;
        unsigned char* bob_message_key = bob.message_receive(alice.get_public_key());
        std::cout << "  Bob's message key: " << bin2hex(bob_message_key, crypto_kdf_KEYBYTES) << std::endl;
        
        // Bob sends to Alice
        std::cout << "Bob sending message:" << std::endl;
        unsigned char* bob_response_key = bob.message_send();
        std::cout << "  Bob's message key: " << bin2hex(bob_response_key, crypto_kdf_KEYBYTES) << std::endl;
        std::cout << "  Bob's new public key: " << bin2hex(bob.get_public_key(), crypto_kx_PUBLICKEYBYTES) << std::endl;
        
        // Alice receives from Bob
        std::cout << "Alice receiving message:" << std::endl;
        unsigned char* alice_response_key = alice.message_receive(bob.get_public_key());
        std::cout << "  Alice's message key: " << bin2hex(alice_response_key, crypto_kdf_KEYBYTES) << std::endl;
        
        // Cleanup
        delete[] alice_message_key;
        delete[] bob_message_key;
        delete[] bob_response_key;
        delete[] alice_response_key;
    }
}

int main(int argc, char *argv[]) {
    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return 1;
    }
    
    // Test Double Ratchet directly
    test_double_ratchet();
    
    return 0;
}