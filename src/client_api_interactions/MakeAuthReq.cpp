#include <sodium.h>
#include <iostream>
#include <map>
#include "../libraries/HTTPSClient.h"
#include "../algorithms/algorithms.h"

std::string post_auth(const std::string& data, const std::string& endpoint = "/") {

    unsigned char public_key[crypto_sign_PUBLICKEYBYTES];
    unsigned char private_key[crypto_sign_SECRETKEYBYTES];
    unsigned char session_key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];

    // db.get_public_key();
    // db.get_private_key();
    // db.get_session_key();

    const char* env_api_host = std::getenv("API_HOST");
    if (env_api_host == nullptr) {
        std::cerr << "API_HOST environment variable is not set" << std::endl;
        return "";
    }

    const std::string API_HOST(env_api_host);
    const std::string API_PATH = API_HOST + endpoint;

    // Generate nonce
    unsigned char nonce[NONCE_LEN];
    randombytes_buf(nonce, sizeof nonce);

    // Create nonce signed with private key
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, nonce, sizeof(nonce), private_key);

    std::string request_body = data;

    // convert headers to base64
    char b64_public_key[crypto_sign_PUBLICKEYBYTES * 2];
    char b64_signature[crypto_sign_BYTES * 2];
    char b64_session_key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES * 2];

    sodium_bin2base64(b64_public_key, sizeof(b64_public_key),
                    public_key, crypto_sign_PUBLICKEYBYTES,
                    sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    sodium_bin2base64(b64_signature, sizeof(b64_signature),
                    signature, crypto_sign_BYTES,
                    sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    sodium_bin2base64(b64_session_key, sizeof(b64_session_key),
                    session_key, crypto_aead_xchacha20poly1305_ietf_KEYBYTES,
                    sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    // `Content-Type` header to `application/json`
    // tells the server that the data being sent is in JSON format
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"public_key", b64_public_key},
        {"signature", b64_signature},
        {"session_key", b64_session_key}
    };

    webwood::HTTPSClient httpsclient;
    std::string response = httpsclient.post(API_HOST, API_PATH, request_body, headers);

    return response;
}

std::string get_auth(const std::string& endpoint = "/") {

    // db.get_public_key();
    // db.get_private_key();
    // db.get_session_key();

    const char* env_api_host = std::getenv("API_HOST");
    if (env_api_host == nullptr) {
        std::cerr << "API_HOST environment variable is not set" << std::endl;
        return "";
    }

    const std::string API_HOST(env_api_host);
    const std::string API_PATH = API_HOST + endpoint;

    // Generate nonce
    unsigned char nonce[NONCE_LEN];
    randombytes_buf(nonce, sizeof nonce);

    // Create nonce signed with private key
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, nonce, sizeof(nonce), private_key);

    // convert headers to base64
    char b64_public_key[crypto_sign_PUBLICKEYBYTES * 2];
    char b64_signature[crypto_sign_BYTES * 2];
    char b64_session_key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES * 2];

    sodium_bin2base64(b64_public_key, sizeof(b64_public_key),
                    public_key, crypto_sign_PUBLICKEYBYTES,
                    sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    sodium_bin2base64(b64_signature, sizeof(b64_signature),
                    signature, crypto_sign_BYTES,
                    sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    sodium_bin2base64(b64_session_key, sizeof(b64_session_key),
                    session_key, crypto_aead_xchacha20poly1305_ietf_KEYBYTES,
                    sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    // `Content-Type` header to `application/json`
    // tells the server that the data being sent is in JSON format
    std::map<std::string, std::string> headers = {
        {"public_key", b64_public_key},
        {"signature", b64_signature},
        {"session_key", b64_session_key}
    };

    webwood::HTTPSClient httpsclient;
    std::string response = httpsclient.get(API_HOST, API_PATH, headers);

    return response;
}