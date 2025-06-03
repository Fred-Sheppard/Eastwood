#include <sodium.h>
#include <iostream>
#include <vector>
#include "XChaCha20-Poly1305.h"
#include <algorithm>
#include "src/keys/secure_memory_buffer.h"
#include "src/algorithms/constants.h"
#include "src/algorithms/algorithms.h"

std::vector<unsigned char> encrypt_message_given_key(const unsigned char* message, const size_t MESSAGE_LEN, const unsigned char* key) {
    // uses crypto_aead_chacha20poly1305_IETF_NPUBBYTES is 24 bytes - nonce
    unsigned char nonce[CHA_CHA_NONCE_LEN];
    randombytes_buf(nonce, sizeof nonce);

    // uses crypto_aead_chacha20poly1305_IETF_ABYTES is 16 bytes - auth tag for mac
    std::vector<unsigned char> ciphertext(MESSAGE_LEN + ENC_OVERHEAD);
    unsigned long long ciphertext_len;

    crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext.data(), &ciphertext_len,
        message, MESSAGE_LEN,
        nullptr, 0, nullptr, nonce, key);

    // Copies the nonce to the start of the result vector - needed for decryption
    std::vector<unsigned char> result(sizeof(nonce) + ciphertext_len);

    std::copy_n(nonce, sizeof(nonce), result.begin());
    std::copy_n(ciphertext.data(), ciphertext_len, result.begin() + sizeof(nonce));

    return result;
}

std::vector<unsigned char> decrypt_message_given_key(const unsigned char* encrypted_data, size_t ENCRYPTED_LEN, const unsigned char* key) {
    if (ENCRYPTED_LEN < CHA_CHA_NONCE_LEN) {
        return {};
    }

    unsigned char nonce[CHA_CHA_NONCE_LEN];
    std::copy_n(encrypted_data, sizeof(nonce), nonce);

    const unsigned char* ciphertext = encrypted_data + sizeof(nonce);
    size_t ciphertext_len = ENCRYPTED_LEN - sizeof(nonce);
    
    if (ciphertext_len < ENC_OVERHEAD) {
        return {};
    }

    std::vector<unsigned char> plaintext(ciphertext_len - ENC_OVERHEAD);
    unsigned long long plaintext_len;

    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            plaintext.data(), &plaintext_len,
            nullptr,
            ciphertext, ciphertext_len,
            nullptr, 0,
            nonce, key) != 0) {
        return {};
    }

    plaintext.resize(plaintext_len);
    return plaintext;
}

// encrypts a message with a random key
std::vector<unsigned char> encrypt_message_auto_key(const unsigned char* message, const size_t MESSAGE_LEN) {
    auto key = SecureMemoryBuffer::create(SYM_KEY_LEN);
    crypto_aead_chacha20poly1305_keygen(key->data());

    unsigned char nonce[CHA_CHA_NONCE_LEN];
    randombytes_buf(nonce, sizeof nonce);

    std::vector<unsigned char> ciphertext(MESSAGE_LEN + ENC_OVERHEAD);
    unsigned long long ciphertext_len;

    crypto_aead_chacha20poly1305_encrypt(
        ciphertext.data(), &ciphertext_len,
        message, MESSAGE_LEN,
        nullptr, 0,
        nullptr, nonce, key->data()
    );

    unsigned char key_nonce[CHA_CHA_NONCE_LEN];
    randombytes_buf(key_nonce, sizeof key_nonce);

    const auto encrypted_key = encrypt_symmetric_key(key, key_nonce);

    std::vector<unsigned char> result(sizeof(nonce) + sizeof(key_nonce) + encrypted_key->size() + ciphertext_len);
    std::copy_n(nonce, sizeof(nonce), result.begin());
    std::copy_n(key_nonce, sizeof(key_nonce), result.begin() + sizeof(nonce));
    std::copy_n(encrypted_key->data(), encrypted_key->size(), result.begin() + sizeof(nonce) + sizeof(key_nonce));
    std::copy_n(ciphertext.data(), ciphertext_len, result.begin() + sizeof(nonce) + sizeof(key_nonce) + encrypted_key->size());

    return result;
}

std::vector<unsigned char> decrypt_message_auto_key(const unsigned char* encrypted_data, size_t ENCRYPTED_LEN) {
    if (ENCRYPTED_LEN < 2*CHA_CHA_NONCE_LEN + ENC_SYM_KEY_LEN) {
        return {};
    }

    unsigned char message_nonce[CHA_CHA_NONCE_LEN];
    std::copy_n(encrypted_data, CHA_CHA_NONCE_LEN, message_nonce);

    unsigned char key_nonce[CHA_CHA_NONCE_LEN];
    std::copy_n(encrypted_data + CHA_CHA_NONCE_LEN, CHA_CHA_NONCE_LEN, key_nonce);

    auto encrypted_key = SecureMemoryBuffer::create(ENC_SYM_KEY_LEN);
    std::copy_n(encrypted_data + 2*CHA_CHA_NONCE_LEN, ENC_SYM_KEY_LEN, encrypted_key->data());

    auto message_key = decrypt_symmetric_key(encrypted_key->data(), key_nonce);

    const unsigned char* ciphertext = encrypted_data + 2*CHA_CHA_NONCE_LEN + ENC_SYM_KEY_LEN;
    size_t ciphertext_len = ENCRYPTED_LEN - 2*CHA_CHA_NONCE_LEN - ENC_SYM_KEY_LEN;
    
    if (ciphertext_len < ENC_OVERHEAD) {
        return {};
    }

    std::vector<unsigned char> plaintext(ciphertext_len - ENC_OVERHEAD);
    unsigned long long plaintext_len;

    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            plaintext.data(), &plaintext_len,
            nullptr,
            ciphertext, ciphertext_len,
            nullptr, 0,
            message_nonce, message_key->data()) != 0) {
        return {};
    }

    plaintext.resize(plaintext_len);
    return plaintext;
}
