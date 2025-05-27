#include "endpoints.h"

#include <sqlite3.h>
#include <nlohmann/json.hpp>

#include "src/key_exchange/utils.h"
#include "src/utils/ConversionUtils.h"
#include "src/client_api_interactions/MakeAuthReq.h"
#include "src/sql/queries.h"

using json = nlohmann::json;

void post_register_user(
    const std::string &username,
    unsigned char pk_identity[crypto_sign_PUBLICKEYBYTES],
    unsigned char registration_nonce[NONCE_LEN],
    unsigned char nonce_signature[crypto_sign_BYTES]
) {
    json body = {
        {"username", username},
        {"identity_public", bin2hex(pk_identity, crypto_sign_PUBLICKEYBYTES)},
        {"nonce", bin2hex(registration_nonce, NONCE_LEN)},
        {"nonce_signature", bin2hex(nonce_signature, crypto_sign_BYTES)}
    };

    post_auth(body, "/registerUser");
};

void post_register_device(
    unsigned char pk_id[crypto_sign_PUBLICKEYBYTES],
    unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    unsigned char pk_signature[crypto_sign_BYTES]
) {
    json body = {
        {"identity_public", bin2hex(pk_id, crypto_sign_PUBLICKEYBYTES)},
        {"device_public", bin2hex(pk_device, crypto_sign_PUBLICKEYBYTES)},
        {"signature", bin2hex(pk_signature, crypto_sign_BYTES)}
    };
    post_auth(body, "/registerDevice");
};

void get_messages(SessionManager manager) {
    std::tuple<QByteArray, QByteArray> keypair = get_keypair("device");
    std::string device_key = std::get<0>(keypair).toStdString();
    //TODO: get vector of messages and parse individually and send on through identity
    json response = get_auth("/getMessages/"+device_key);

    DeviceMessage msg;
    msg.header = new MessageHeader();

    std::vector<uint8_t> device_id = hex_string_to_binary(response["device_id"]);
    std::vector<uint8_t> dh_public = hex_string_to_binary(response["dh_public"]);
    std::vector<uint8_t> ciphertext = hex_string_to_binary(response["ciphertext"]);

    std::copy(device_id.begin(), device_id.end(), msg.header->device_id);
    std::copy(dh_public.begin(), dh_public.end(), msg.header->dh_public);
    msg.header->prev_chain_length = response["prev_chain_length"];
    msg.header->message_index = response["message_index"];
    
    // Allocate and copy ciphertext
    msg.ciphertext = new unsigned char[ciphertext.size()];
    std::copy(ciphertext.begin(), ciphertext.end(), msg.ciphertext);
    msg.length = ciphertext.size();
    
    // Get the other identity key from the response
    std::vector<uint8_t> other_identity = hex_string_to_binary(response["other_identity"]);
    unsigned char* other_pk = new unsigned char[other_identity.size()];
    std::copy(other_identity.begin(), other_identity.end(), other_pk);

    // Route the message to the identity session
    manager.routeToIdentity(msg, other_pk);
    
    delete[] other_pk;
}

void post_ratchet_message(const DeviceMessage* msg) {
    json body = {
        {"device_id", bin_to_hex(msg->header->device_id, sizeof(msg->header->device_id))},
        {"dh_public", bin_to_hex(msg->header->dh_public, sizeof(msg->header->dh_public))},
        {"prev_chain_length", msg->header->prev_chain_length},
        {"prev_chain_length", msg->header->message_index},
        {"ciphertext", bin_to_hex(msg->ciphertext, sizeof(msg->ciphertext))},
    };
    //todo: post to /sendMessage/deviceId

    post_auth(body, "/sendMessage");
};

keyBundleRequest get_keybundles(unsigned char pk_identity[32]) {
    std::string hex_pk_identity = bin2hex(pk_identity, 32);
    json response = get_auth("/keybundle/"+hex_pk_identity);

    keyBundleRequest request;
    
    // Allocate and copy my identity public key
    request.my_identity_public = new unsigned char[32];
    memcpy(request.my_identity_public, pk_identity, 32);
    
    // Allocate and copy their identity public key
    std::vector<uint8_t> their_identity = hex_string_to_binary(response["their_identity_public"]);
    request.their_identity_public = new unsigned char[32];
    memcpy(request.their_identity_public, their_identity.data(), 32);
    
    // Parse key bundles from response
    for (const auto& bundle : response["key_bundles"]) {
        keyBundle kb;
        
        // Allocate and copy device public key
        std::vector<uint8_t> device_public = hex_string_to_binary(bundle["device_public"]);
        kb.device_key_public = new unsigned char[32];
        memcpy(kb.device_key_public, device_public.data(), 32);
        
        // Allocate and copy signed prekey public key
        std::vector<uint8_t> signed_prekey = hex_string_to_binary(bundle["signed_prekey_public"]);
        kb.signed_prekey_public = new unsigned char[32];
        memcpy(kb.signed_prekey_public, signed_prekey.data(), 32);
        
        // Allocate and copy signed prekey signature
        std::vector<uint8_t> signature = hex_string_to_binary(bundle["signed_prekey_signature"]);
        kb.signed_prekey_signature = new unsigned char[64];
        memcpy(kb.signed_prekey_signature, signature.data(), 64);
        
        // Allocate and copy one-time prekey public key
        std::vector<uint8_t> one_time_prekey = hex_string_to_binary(bundle["one_time_prekey_public"]);
        kb.onetime_prekey_public = new unsigned char[32];
        memcpy(kb.onetime_prekey_public, one_time_prekey.data(), 32);
        
        request.key_bundles.push_back(kb);
    }

    return request;
}


void post_handshake_device(
    const unsigned char* recipient_device_key_public,
    const unsigned char* recipient_signed_prekey_public,
    const unsigned char* recipient_onetime_prekey_public,
    const unsigned char* my_device_key_public,
    const unsigned char* my_ephemeral_key_public
) {
    json body = {
        {"recipient_device_key", bin2hex(recipient_device_key_public, crypto_box_PUBLICKEYBYTES)},
        {"recipient_signed_prekey", bin2hex(recipient_signed_prekey_public, crypto_box_PUBLICKEYBYTES)},
        {"recipient_onetime_prekey", bin2hex(recipient_onetime_prekey_public, crypto_box_PUBLICKEYBYTES)},
        {"my_device_key", bin2hex(my_device_key_public, crypto_box_PUBLICKEYBYTES)},
        {"my_ephemeral_key", bin2hex(my_ephemeral_key_public, crypto_box_PUBLICKEYBYTES)}
    };
    post_auth(body, "/handshake");
}

