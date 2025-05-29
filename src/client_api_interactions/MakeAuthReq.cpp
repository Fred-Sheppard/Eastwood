#include <sodium.h>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include "../libraries/HTTPSClient.h"
#include "../utils/ConversionUtils.h"
#include "../utils/JsonParser.h"
#include "../sql/queries.h"
#include "src/keys/session_token_manager.h"
#include "src/key_exchange/utils.h"
#include "src/utils/utils.h"

using json = nlohmann::json;

std::string pk_device_hex() {
    auto pk_device = get_public_key("device");
    return bin2hex(reinterpret_cast<const unsigned char *>(pk_device.data()), pk_device.size());
}

/**
 *
 * @param message The json body or nonce to be signed
 * @return The hex-encoded signature
 */
std::string sign_message(const std::string &message) {
    const auto sk_device = get_decrypted_sk("device");

    // Check if message is hex encoded
    bool is_hex = true;
    for (char c : message) {
        if (!std::isxdigit(c)) {
            is_hex = false;
            break;
        }
    }

    unsigned char signature[crypto_sign_BYTES];
    if (is_hex) {
        // Convert hex to binary first
        std::vector<unsigned char> binary(message.length() / 2);
        if (!hex_to_bin(message, binary.data(), binary.size())) {
            throw std::runtime_error("Failed to convert hex message to binary");
        }
        crypto_sign_detached(
            signature, nullptr,
            binary.data(),
            binary.size(),
            sk_device->data()
        );
    } else {
        // Sign raw binary data
        crypto_sign_detached(
            signature, nullptr,
            reinterpret_cast<const unsigned char *>(message.data()),
            message.length(),
            sk_device->data()
        );
    }
    return bin2hex(signature, crypto_sign_BYTES);
}

std::string generate_get_headers(const std::string &nonce) {
    auto pk_device = get_public_key("device");
    std::string pk_device_hex = bin2hex(reinterpret_cast<const unsigned char *>(pk_device.data()), pk_device.size());

    auto hex_signature = sign_message(nonce);
    std::map<std::string, std::string> headers = {
        {"device_public", pk_device_hex},
        {"signature", hex_signature},
        {"nonce", nonce},
        {"session_token", SessionTokenManager::instance().getToken()}
    };

    // Convert the map of headers into a single string
    std::string header_string;
    for (const auto &[key, value]: headers) {
        header_string.append(key).append(": ").append(value).append("\r\n");
    }
    return header_string;
}


json generate_post_headers(const std::string &request_body) {
    auto hex_signature = sign_message(request_body);

    std::map<std::string, std::string> headers = {
        {"device_public", pk_device_hex()},
        {"signature", hex_signature},
        {"session_token", SessionTokenManager::instance().getToken()}
    };

    // Convert the map of headers into a single string
    std::string header_string;
    for (const auto &[key, value]: headers) {
        header_string.append(key).append(": ").append(value).append("\n");
    }
    return header_string;
}

json handle_response(const std::string &response) {
    try {
        qDebug() << response;
        return webwood::parse_json_response(response);
    } catch (const webwood::HttpError &e) {
        throw webwood::HttpError(webwood::extract_response_body(response), e.get_status_code());
    } catch (const std::exception &) {
        throw webwood::HttpError(webwood::extract_response_body(response), 0);
    }
}

std::string generate_hex_nonce() {
    unsigned char nonce[CHA_CHA_NONCE_LEN];
    randombytes_buf(nonce, CHA_CHA_NONCE_LEN);

    constexpr auto max_len = CHA_CHA_NONCE_LEN * 2 + 1;  // Each byte becomes 2 hex chars + null terminator
    char hex_nonce[max_len];
    sodium_bin2hex(hex_nonce, max_len, nonce, CHA_CHA_NONCE_LEN);
    return std::string(hex_nonce);
}


json post(const json &data, const std::string &endpoint = "/") {
    const std::string API_HOST = load_env_variable("API_HOST");
    if (API_HOST.empty()) {
        std::cerr << "API_HOST not found in .env file" << std::endl;
        throw;
    }

    std::string hex_nonce = generate_hex_nonce();
    std::cout << hex_nonce;

    json request_json = data;
    request_json["nonce"] = hex_nonce;
    const std::string request_body = request_json.dump();
    qDebug().noquote() << "request.dump" << request_json.dump();
    const std::string headers = generate_post_headers(request_body);

    webwood::HTTPSClient client;
    const std::string response = client.post(API_HOST, endpoint, headers, request_body);
    return handle_response(response);
}


json get(const std::string &endpoint = "/") {
    const std::string API_HOST = load_env_variable("API_HOST");
    if (API_HOST.empty()) {
        std::cerr << "API_HOST not found in .env file" << std::endl;
        throw;
    }

    const std::string hex_nonce = generate_hex_nonce();

    const std::string headers = generate_get_headers(hex_nonce);

    webwood::HTTPSClient client;
    const std::string response = client.get(API_HOST, endpoint, headers);
    return handle_response(response);
}
