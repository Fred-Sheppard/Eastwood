#include "endpoints.h"
#include <nlohmann/json.hpp>
#include "src/key_exchange/utils/ConversionUtils.h"

#include "src/key_exchange/utils.h"

using json = nlohmann::json;

void post_register_user(
    const std::string &username,
    unsigned char pk_identity[crypto_sign_PUBLICKEYBYTES],
    unsigned char registration_nonce[NONCE_LEN],
    unsigned char nonce_signature[crypto_sign_BYTES]
) {

    //todo change to hex
    json body = {
        {"username", username},
        {"identity_public", bin2base64(pk_identity, crypto_sign_PUBLICKEYBYTES)},
        {"nonce", bin2base64(registration_nonce, NONCE_LEN)},
        {"nonce_signature", bin2base64(nonce_signature, crypto_sign_BYTES)}
    };
    // TODO:  post("/registerUser", body.dump());
};

void post_register_device(
    unsigned char pk_id[crypto_sign_PUBLICKEYBYTES],
    unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    unsigned char pk_signature[crypto_sign_BYTES]
) {

    //todo: change to hex
    json body = {
        {"identity_public", bin2base64(pk_id, crypto_sign_PUBLICKEYBYTES)},
        {"device_public", bin2base64(pk_device, crypto_sign_PUBLICKEYBYTES)},
        {"signature", bin2base64(pk_signature, crypto_sign_BYTES)}
    };
    // TODO:  post("/registerDevice", body.dump());
};

Message* get_messages() {
    // TODO: GET /getMessages/device

    //TODO: get vector of messages and parse individually and send on through identity
    json response = json::parse(""); // Replace with actual API response
    
    Message* msg = new Message();
    msg->header = std::make_unique<MessageHeader>();
    
    std::vector<uint8_t> device_session_id = hex_string_to_binary(response["device_session_id"]);
    std::vector<uint8_t> dh_public = hex_string_to_binary(response["dh_public"]);
    std::vector<uint8_t> ciphertext = hex_string_to_binary(response["ciphertext"]);
    
    std::copy(device_session_id.begin(), device_session_id.end(), msg->header->device_session_id.begin());
    std::copy(dh_public.begin(), dh_public.end(), msg->header->dh_public.begin());
    msg->header->prev_chain_length = response["prev_chain_length"];
    msg->header->message_index = response["message_index"];
    std::copy(ciphertext.begin(), ciphertext.end(), msg->message.begin());
    
    return msg;

    // todo: route through identity session?
}

void post_ratchet_message(const Message* msg, std::vector<uint8_t> device_id) {
    json body = {
        {"device_session_id", bin_to_hex(msg->header->device_session_id.data(), sizeof(msg->header->device_session_id.data()))},
        {"dh_public", bin_to_hex(msg->header->dh_public.data(), sizeof(msg->header->dh_public.data()))},
        {"prev_chain_length", msg->header->prev_chain_length},
        {"prev_chain_length", msg->header->message_index},
        {"ciphertext", bin_to_hex(msg->message.data(), sizeof(msg->message.data()))},
    };

    //todo: post to /sendMessage/deviceId
}
