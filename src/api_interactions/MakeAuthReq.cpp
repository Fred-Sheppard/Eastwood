#include <sodium.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "../libraries/HTTPSClient.h"

bool make_auth_request(const std::string& api_url, const unsigned char* public_key, const unsigned char* private_key, const unsigned char* session_key, const std::string& data) {
    
    // Generate nonce
    unsigned char nonce[32];
    randombytes_buf(nonce, sizeof nonce);

    // Create nonce signed with private key
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, nonce, sizeof(nonce), private_key);


    nlohmann::json payload = {
        {"public_key", std::string(reinterpret_cast<const char*>(public_key), crypto_sign_PUBLICKEYBYTES)},
        {"signature", std::string(reinterpret_cast<const char*>(signature), crypto_sign_BYTES)},
        {"session_key", std::string(reinterpret_cast<const char*>(session_key), crypto_aead_xchacha20poly1305_ietf_KEYBYTES)},
        {"data", data}
    };

    std::string request_body = payload.dump();

    // `Content-Type` header to `application/json`
    // tells the server that the data being sent is in JSON format
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"}
    };

    // Nialls https client
    webwood::HTTPSClient httpsclient;
    std::string response = httpsclient.post(api_url, request_body);

    return !response.empty() && response.find("error") == std::string::npos;
}