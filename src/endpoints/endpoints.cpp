#include "endpoints.h"

#include <nlohmann/json.hpp>

#include "src/key_exchange/utils.h"
#include "src/utils/ConversionUtils.h"
#include "src/client_api_interactions/MakeAuthReq.h"
#include "src/sql/queries.h"
#include "src/client_api_interactions/MakeUnauthReq.h"
#include "src/keys/session_token_manager.h"
#include "src/sessions/IdentityManager.h"
#include "src/utils/utils.h"

using json = nlohmann::json;

void post_register_user(
    const std::string &username,
    const unsigned char pk_identity[crypto_sign_PUBLICKEYBYTES],
    const unsigned char registration_nonce[CHA_CHA_NONCE_LEN],
    const unsigned char nonce_signature[crypto_sign_BYTES]
) {
    const json body = {
        {"username", username},
        {"identity_public", bin2hex(pk_identity, crypto_sign_PUBLICKEYBYTES)},
        {"nonce", bin2hex(registration_nonce, CHA_CHA_NONCE_LEN)},
        {"nonce_signature", bin2hex(nonce_signature, crypto_sign_BYTES)}
    };

    post_unauth("/registerUser", body);
};

void post_register_device(
    const unsigned char pk_id[crypto_sign_PUBLICKEYBYTES],
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    const unsigned char pk_signature[crypto_sign_BYTES]
) {
    const json body = {
        {"identity_public", bin2hex(pk_id, crypto_sign_PUBLICKEYBYTES)},
        {"device_public", bin2hex(pk_device, crypto_sign_PUBLICKEYBYTES)},
        {"signature", bin2hex(pk_signature, crypto_sign_BYTES)}
    };
    post_unauth("/registerDevice", body);
};

std::vector<unsigned char> post_request_login(
    std::string username,
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES]
) {
    qDebug() << "Requesting login nonce from server";
    const json body = {
        {"username", username},
        {"device_public", bin2hex(pk_device, crypto_sign_PUBLICKEYBYTES)}
    };
    const json response = post_unauth("/requestLogin", body);
    const std::string nonce_string = response["data"]["nonce"];

    // Allocate vector of correct size
    std::vector<unsigned char> nonce_vec(nonce_string.length() / 2);

    // Convert hex to bin
    if (!hex_to_bin(nonce_string, nonce_vec.data(), nonce_vec.size())) {
        throw std::runtime_error("Failed to decode nonce when logging in");
    }

    return nonce_vec;
}

std::string post_authenticate(
    std::string username,
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    unsigned char signature[crypto_sign_BYTES]
) {
    qDebug() << "Authenticating user";
    const json body = {
        {"username", username},
        {"device_public", bin2hex(pk_device, crypto_sign_PUBLICKEYBYTES)},
        {"nonce_signature", bin2hex(signature, crypto_sign_BYTES)}
    };
    const json response = post_unauth("/authenticate", body);
    return response["data"]["token"];
}

std::vector<std::tuple<std::string, DeviceMessage *> > get_messages() {
    json response = get("/incomingMessages");

    std::cout << "Raw response: " << response.dump() << std::endl;
    std::cout << "Response keys: ";
    for (auto &[key, value]: response.items()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    std::vector<std::tuple<std::string, DeviceMessage *> > messages;

    // Check if data array exists
    if (!response.contains("data") || !response["data"].is_array()) {
        std::cout << "No messages in response" << std::endl;
        return messages;
    }

    for (const auto &message: response["data"]) {
        int ciphertext_length = message["ciphertext_length"].get<int>();

        auto initator_dev_key = new unsigned char[crypto_box_PUBLICKEYBYTES];
        auto new_dh_public = new unsigned char[crypto_box_PUBLICKEYBYTES];
        auto ciphertext = new unsigned char[ciphertext_length];

        std::string username = message["username"].get<std::string>();
        std::string dev_key_str = message["initiator_device_public_key"].get<std::string>();
        std::string dh_pub_str = message["dh_public"].get<std::string>();
        std::string ciphertext_str = message["ciphertext"].get<std::string>();

        int prev_chain_length = message["prev_chain_length"].get<int>();
        int message_index = message["message_index"].get<int>();

        bool success = hex_to_bin(dev_key_str, initator_dev_key, crypto_box_PUBLICKEYBYTES) &&
                       hex_to_bin(dh_pub_str, new_dh_public, crypto_box_PUBLICKEYBYTES) &&
                       hex_to_bin(ciphertext_str, ciphertext, ciphertext_length);

        if (!success) {
            delete[] initator_dev_key;
            delete[] new_dh_public;
            delete[] ciphertext;
            throw std::runtime_error("Failed to decode message data");
        }

        DeviceMessage *msg = new DeviceMessage();
        MessageHeader *header = new MessageHeader();

        memcpy(header->dh_public, new_dh_public, crypto_box_PUBLICKEYBYTES);
        header->message_index = message_index;
        header->prev_chain_length = prev_chain_length;
        memcpy(header->device_id, initator_dev_key, crypto_box_PUBLICKEYBYTES);

        msg->header = header;
        msg->ciphertext = ciphertext;
        msg->length = ciphertext_length;

        messages.push_back(std::make_tuple(username, msg));
    }
    return messages;
}

void post_ratchet_message(std::vector<DeviceMessage*> messages) {
    json data = json::object();
    data["messages"] = json::array();

    for (auto msg : messages) {
        const auto dev_pub = new unsigned char[crypto_box_PUBLICKEYBYTES];
        QByteArray dev_pub_byte = get_public_key("device");
        memcpy(dev_pub, dev_pub_byte.constData(), crypto_box_PUBLICKEYBYTES);

        json body = json::object();
        body["file_id"] = 0; // update
        body["initiator_device_public_key"] = bin2hex(dev_pub, crypto_box_PUBLICKEYBYTES);
        body["recipient_device_public_key"] = bin2hex(msg->header->device_id, crypto_box_PUBLICKEYBYTES);
        body["dh_public"] = bin2hex(msg->header->dh_public, crypto_box_PUBLICKEYBYTES);
        body["prev_chain_length"] = msg->header->prev_chain_length;
        body["message_index"] = msg->header->message_index;
        body["ciphertext"] = bin2hex(msg->ciphertext, msg->length);
        body["ciphertext_length"] = msg->length;

        data["messages"].push_back(body);
        delete[] dev_pub;
    }

    post("/sendMessage", data);
}

std::vector<KeyBundle*> get_keybundles(const std::string &username) {
    json response = get("/keybundle/" + username);

    // Get my identity public key
    std::string my_identity_public_hex = response["data"]["identity_public_key"];
    auto my_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
    if (!hex_to_bin(my_identity_public_hex, my_identity_public, crypto_sign_PUBLICKEYBYTES)) {
        delete[] my_identity_public;
        throw std::runtime_error("Failed to decode my identity public key");
    }

    // Get my device and signed prekey private keys from database
    const auto [pk_device, sk_device] = get_decrypted_keypair("device");
    const auto [pk_signed, sk_signed] = get_decrypted_keypair("signed");

    std::vector<KeyBundle *> bundles;
    std::string their_identity_public_hex;

    // Process each key bundle
    for (const auto &bundle: response["data"]["key_bundles"]) {
        std::string their_device_public_hex = bundle["device_public_key"];
        their_identity_public_hex = bundle["identity_public_key"];
        std::string their_signed_public_hex = bundle["signedpre_key"];
        std::string their_signed_signature_hex = bundle["signedpk_signature"];

        // Allocate memory for binary data
        auto their_device_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        auto their_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        auto their_signed_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        auto their_signed_signature = new unsigned char[crypto_sign_BYTES];
        unsigned char* their_onetime_public = nullptr;

        bool device_ok = hex_to_bin(their_device_public_hex, their_device_public, crypto_sign_PUBLICKEYBYTES);
        bool identity_ok = hex_to_bin(their_identity_public_hex, their_identity_public, crypto_sign_PUBLICKEYBYTES);
        bool signed_ok = hex_to_bin(their_signed_public_hex, their_signed_public, crypto_sign_PUBLICKEYBYTES);
        bool signature_ok = hex_to_bin(their_signed_signature_hex, their_signed_signature, crypto_sign_BYTES);

        // Only process one-time key if it exists and is not NULL
        if (bundle.contains("one_time_key") && !bundle["one_time_key"].is_null()) {
            std::string their_onetime_public_hex = bundle["one_time_key"];
            their_onetime_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
            bool onetime_ok = hex_to_bin(their_onetime_public_hex, their_onetime_public, crypto_sign_PUBLICKEYBYTES);
            std::cout << "One-time key conversion: " << (onetime_ok ? "success" : "failed") << std::endl;
            if (!onetime_ok) {
                delete[] their_onetime_public;
                their_onetime_public = nullptr;
            }
        }

        if (!device_ok || !identity_ok || !signed_ok || !signature_ok) {
            // Clean up on error
            delete[] their_device_public;
            delete[] their_identity_public;
            delete[] their_signed_public;
            delete[] their_signed_signature;
            if (their_onetime_public) {
                delete[] their_onetime_public;
            }
            throw std::runtime_error("Failed to decode key bundle data");
        }

        auto pk_eph = new unsigned char[crypto_sign_BYTES];
        auto sk_buffer_eph = SecureMemoryBuffer::create(ENC_SECRET_KEY_LEN);
        crypto_box_keypair(pk_eph, sk_buffer_eph->data());

        // Create a new KeyBundle
        auto *key_bundle = new SendingKeyBundle(
            reinterpret_cast<unsigned char *>(const_cast<char *>(pk_device.constData())),
            pk_eph,
            std::shared_ptr<SecureMemoryBuffer>(sk_buffer_eph.release()),
            their_device_public,
            their_signed_public,
            their_onetime_public,
            their_signed_signature
        );

        bundles.push_back(key_bundle);
    }

    // Convert their identity public key to binary
    auto their_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
    if (!hex_to_bin(their_identity_public_hex, their_identity_public, crypto_sign_PUBLICKEYBYTES)) {
        delete[] their_identity_public;
        throw std::runtime_error("Failed to decode their identity public key");
    }

    return bundles;
}

void post_handshake_device(
    const unsigned char *recipient_device_key_public,
    const unsigned char *recipient_signed_prekey_public,
    const unsigned char *recipient_signed_prekey_signature,
    const unsigned char *recipient_onetime_prekey_public,
    const unsigned char *my_device_key_public,
    const unsigned char *my_ephemeral_key_public
) {
    json body = {
        {"recipient_device_key", bin2hex(recipient_device_key_public, crypto_box_PUBLICKEYBYTES)},
        {"recipient_signed_public_prekey", bin2hex(recipient_signed_prekey_public, crypto_box_PUBLICKEYBYTES)},
        {
            "recipient_signed_public_prekey_signature",
            bin2hex(recipient_signed_prekey_signature, crypto_box_PUBLICKEYBYTES)
        },
        {"initiator_ephemeral_public_key", bin2hex(my_ephemeral_key_public, crypto_box_PUBLICKEYBYTES)},
        {"initiator_device_public_key", bin2hex(my_device_key_public, crypto_box_PUBLICKEYBYTES)},
    };

    // Only add one-time prekey if it exists
    if (recipient_onetime_prekey_public != nullptr) {
        body["recipient_onetime_public_prekey"] = bin2hex(recipient_onetime_prekey_public, crypto_box_PUBLICKEYBYTES);
    }

    post("/handshake", body);
}

std::vector<std::tuple<std::string, KeyBundle *> > get_handshake_backlog() {
    json response = get("/incomingHandshakes");
    std::cout << "Raw response: " << response.dump() << std::endl;
    std::cout << "Response keys: ";
    for (auto &[key, value]: response.items()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    std::vector<std::tuple<std::string, KeyBundle *> > bundles;

    for (const auto &handshake: response["data"]) {
        auto initator_dev_key = new unsigned char[crypto_box_PUBLICKEYBYTES];
        auto initiator_eph_pub = new unsigned char[crypto_box_PUBLICKEYBYTES];
        unsigned char* recip_onetime_pub = nullptr;

        std::string dev_key_str = handshake["initiator_device_public_key"].get<std::string>();
        std::string eph_pub_str = handshake["initiator_ephemeral_public_key"].get<std::string>();
        std::string username = handshake["username"].get<std::string>();

        bool success = hex_to_bin(dev_key_str, initator_dev_key, crypto_box_PUBLICKEYBYTES) &&
                       hex_to_bin(eph_pub_str, initiator_eph_pub, crypto_box_PUBLICKEYBYTES)
        if (!success) {
            delete[] initator_dev_key;
            delete[] initiator_eph_pub;
            throw std::runtime_error("Failed to decode handshake backlog data");
        }

        // Only process one-time prekey if it exists in the response
        if (handshake.contains("recipient_onetime_public_prekey")) {
            std::string onetime_pub_str = handshake["recipient_onetime_public_prekey"].get<std::string>();
            recip_onetime_pub = new unsigned char[crypto_box_PUBLICKEYBYTES];
            if (!hex_to_bin(onetime_pub_str, recip_onetime_pub, crypto_box_PUBLICKEYBYTES)) {
                delete[] initator_dev_key;
                delete[] initiator_eph_pub;
                delete[] recip_onetime_pub;
                throw std::runtime_error("Failed to decode one-time prekey data");
            }
        }

        auto device_key = get_public_key("device");
        auto new_bundle = new ReceivingKeyBundle(
            initator_dev_key,
            initiator_eph_pub,
            const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(device_key.constData())),
            recip_onetime_pub
        );

        bundles.push_back(std::make_tuple(username, new_bundle));
    }

    return bundles;
}

void post_new_keybundles(
    std::tuple<QByteArray, std::unique_ptr<SecureMemoryBuffer> > device_keypair,
    std::tuple<unsigned char *, std::unique_ptr<SecureMemoryBuffer> > signed_prekeypair,
    const std::vector<std::tuple<unsigned char *, std::unique_ptr<SecureMemoryBuffer>, unsigned char *> > &otks
) {
    auto [pk_signed, sk_signed] = std::move(signed_prekeypair);
    auto [pk_device_q, sk_device] = std::move(device_keypair);

    //sign the public key with device key
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, pk_signed, crypto_sign_PUBLICKEYBYTES, sk_device->data());

    // Convert signature to hex string
    std::string signature_hex = bin2hex(signature, crypto_sign_BYTES);
    std::string signed_prekey_pub_hex = bin2hex(pk_signed, crypto_box_PUBLICKEYBYTES);

    // Create JSON payload
    json body = {
        {"signedpre_key", signed_prekey_pub_hex},
        {"signedpk_signature", signature_hex},
        {"one_time_keys", json::array()}
    };

    for (const auto &[pk, sk, nonce]: otks) {
        body["one_time_keys"].push_back(bin2hex(pk, crypto_box_PUBLICKEYBYTES));
    }
    post("/updateKeybundle", body);
}

std::string post_upload_file(std::vector<unsigned char> encrypted_bytes) {
    const json body = {
        {"encrypted_bytes", encrypted_bytes}
    };

    const json response = post("/uploadFile", body);
    return response["data"]["file_id"];
}