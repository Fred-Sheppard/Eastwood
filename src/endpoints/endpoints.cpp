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
        {"identity_public", sodium_bin2hex(new char[crypto_sign_PUBLICKEYBYTES * 2 + 1], crypto_sign_PUBLICKEYBYTES * 2 + 1, pk_identity, crypto_sign_PUBLICKEYBYTES)},
        {"nonce", sodium_bin2hex(new char[CHA_CHA_NONCE_LEN * 2 + 1], CHA_CHA_NONCE_LEN * 2 + 1, registration_nonce, CHA_CHA_NONCE_LEN)},
        {"nonce_signature", sodium_bin2hex(new char[crypto_sign_BYTES * 2 + 1], crypto_sign_BYTES * 2 + 1, nonce_signature, crypto_sign_BYTES)}
    };
    std::cout << body << std::endl;

    post_unauth(body, "/registerUser");
};

void post_register_device(
    const unsigned char pk_id[crypto_sign_PUBLICKEYBYTES],
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    const unsigned char pk_signature[crypto_sign_BYTES]
) {
    const json body = {
        {"identity_public", sodium_bin2hex(new char[crypto_sign_PUBLICKEYBYTES * 2 + 1], crypto_sign_PUBLICKEYBYTES * 2 + 1, pk_id, crypto_sign_PUBLICKEYBYTES)},
        {"device_public", sodium_bin2hex(new char[crypto_sign_PUBLICKEYBYTES * 2 + 1], crypto_sign_PUBLICKEYBYTES * 2 + 1, pk_device, crypto_sign_PUBLICKEYBYTES)},
        {"signature", sodium_bin2hex(new char[crypto_sign_BYTES * 2 + 1], crypto_sign_BYTES * 2 + 1, pk_signature, crypto_sign_BYTES)}
    };
    post_unauth(body, "/registerDevice");
};

std::vector<unsigned char> post_request_login(
    std::string username,
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES]
) {
    qDebug() << "Requesting login nonce from server";
    const json body = {
        {"username", username},
        {"device_public", sodium_bin2hex(new char[crypto_sign_PUBLICKEYBYTES * 2 + 1], crypto_sign_PUBLICKEYBYTES * 2 + 1, pk_device, crypto_sign_PUBLICKEYBYTES)}
    };
    const json response = post_unauth(body, "/requestLogin");
    QString response_text(response.dump().data());
    const std::string nonce_string = response["data"]["nonce"];

    // Allocate vector of correct size
    std::vector<unsigned char> nonce_vec(nonce_string.length() / 2);

    // Convert hex to bin
    size_t bin_len;
    if (sodium_hex2bin(nonce_vec.data(), nonce_vec.size(),
                      nonce_string.c_str(), nonce_string.length(),
                      nullptr, &bin_len, nullptr) != 0 || bin_len != nonce_vec.size()) {
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
        {"device_public", sodium_bin2hex(new char[crypto_sign_PUBLICKEYBYTES * 2 + 1], crypto_sign_PUBLICKEYBYTES * 2 + 1, pk_device, crypto_sign_PUBLICKEYBYTES)},
        {"nonce_signature", sodium_bin2hex(new char[crypto_sign_BYTES * 2 + 1], crypto_sign_BYTES * 2 + 1, signature, crypto_sign_BYTES)}
    };
    const json response = post_unauth(body, "/authenticate");
    return response["data"]["token"];
}

std::vector<DeviceMessage*> get_messages() {
    json response = get("/incomingMessages");

    std::cout << "Raw response: " << response.dump() << std::endl;
    std::cout << "Response keys: ";
    for (auto& [key, value] : response.items()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    std::vector<DeviceMessage*> messages;

    for (const auto& handshake : response["data"]) {
        int ciphertext_length = handshake["ciphertext_length"].get<int>();

        auto initator_dev_key = new unsigned char[crypto_box_PUBLICKEYBYTES];
        auto new_dh_public = new unsigned char[crypto_box_PUBLICKEYBYTES];
        auto ciphertext = new unsigned char[ciphertext_length];

        std::string dev_key_str = handshake["initiator_device_public_key"].get<std::string>();
        std::string dh_pub_str = handshake["dh_public"].get<std::string>();
        std::string ciphertext_str = handshake["ciphertext"].get<std::string>();

        int prev_chain_length = handshake["prev_chain_length"].get<int>();
        int message_index = handshake["message_index"].get<int>();

        bool success = sodium_hex2bin(initator_dev_key, crypto_box_PUBLICKEYBYTES,
                                     dev_key_str.c_str(), dev_key_str.length(),
                                     nullptr, nullptr, nullptr) == 0 &&
            sodium_hex2bin(new_dh_public, crypto_box_PUBLICKEYBYTES,
                          dh_pub_str.c_str(), dh_pub_str.length(),
                          nullptr, nullptr, nullptr) == 0 &&
            sodium_hex2bin(ciphertext, ciphertext_length,
                          ciphertext_str.c_str(), ciphertext_str.length(),
                          nullptr, nullptr, nullptr) == 0;

        if (!success) {
            delete[] initator_dev_key;
            delete[] new_dh_public;
            delete[] ciphertext;
            throw std::runtime_error("Failed to decode handshake backlog data");
        }

        DeviceMessage* msg = new DeviceMessage();
        MessageHeader* header = new MessageHeader();

        memcpy(header->dh_public, new_dh_public, crypto_box_PUBLICKEYBYTES);
        header->message_index = message_index;
        header->prev_chain_length = prev_chain_length;
        memcpy(header->device_id, initator_dev_key, crypto_box_PUBLICKEYBYTES);

        memcpy(msg->header, header, sizeof(MessageHeader));
        memcpy(msg->ciphertext, ciphertext, ciphertext_length);

        messages.push_back(msg);
    }
    return messages;
}

void post_ratchet_message(const DeviceMessage *msg) {
    json body = {
        {"file_id", 0},
        {"recipient_device_public_key", sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, msg->header->device_id, sizeof(msg->header->device_id))},
        {"dh_public", sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, msg->header->dh_public, sizeof(msg->header->dh_public))},
        {"prev_chain_length", msg->header->prev_chain_length},
        {"message_index", msg->header->message_index},
        {"ciphertext", sodium_bin2hex(new char[sizeof(msg->ciphertext) * 2 + 1], sizeof(msg->ciphertext) * 2 + 1, msg->ciphertext, sizeof(msg->ciphertext))},
        {"ciphertext_length", sizeof(msg->ciphertext)},
    };

    post(body, "/sendMessage");
};

void get_keybundles(std::string username) {
    json response = get("/keybundle/" + username);

    // Get my identity public key
    std::string my_identity_public_hex = response["data"]["identity_public_key"];
    unsigned char *my_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
    if (sodium_hex2bin(my_identity_public, crypto_sign_PUBLICKEYBYTES,
                      my_identity_public_hex.c_str(), my_identity_public_hex.length(),
                      nullptr, nullptr, nullptr) != 0) {
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
        // Convert hex strings to binary
        std::string their_device_public_hex = bundle["device_public_key"];
        their_identity_public_hex = bundle["identity_public_key"];
        std::string their_onetime_public_hex = bundle["one_time_key"];
        std::string their_signed_public_hex = bundle["signedpre_key"];
        std::string their_signed_signature_hex = bundle["signedpk_signature"];

        // Allocate memory for binary data
        unsigned char *their_device_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char *their_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char *their_onetime_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char *their_signed_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char *their_signed_signature = new unsigned char[crypto_sign_BYTES]; // Use correct size for signature

        // Convert hex to binary
        std::cout << "Converting hex to binary:" << std::endl;
        std::cout << "Device public key hex length: " << their_device_public_hex.length() << ", expected binary size: "
                << crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "Identity public key hex length: " << their_identity_public_hex.length() <<
                ", expected binary size: " << crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "One-time key hex length: " << their_onetime_public_hex.length() << ", expected binary size: " <<
                crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "Signed prekey hex length: " << their_signed_public_hex.length() << ", expected binary size: " <<
                crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "Signature hex length: " << their_signed_signature_hex.length() << ", expected binary size: " <<
                crypto_sign_BYTES << std::endl;

        bool device_ok = sodium_hex2bin(their_device_public, crypto_sign_PUBLICKEYBYTES,
                                       their_device_public_hex.c_str(), their_device_public_hex.length(),
                                       nullptr, nullptr, nullptr) == 0;
        std::cout << "Device public key conversion: " << (device_ok ? "success" : "failed") << std::endl;

        bool identity_ok = sodium_hex2bin(their_identity_public, crypto_sign_PUBLICKEYBYTES,
                                         their_identity_public_hex.c_str(), their_identity_public_hex.length(),
                                         nullptr, nullptr, nullptr) == 0;
        std::cout << "Identity public key conversion: " << (identity_ok ? "success" : "failed") << std::endl;

        bool onetime_ok = sodium_hex2bin(their_onetime_public, crypto_sign_PUBLICKEYBYTES,
                                        their_onetime_public_hex.c_str(), their_onetime_public_hex.length(),
                                        nullptr, nullptr, nullptr) == 0;
        std::cout << "One-time key conversion: " << (onetime_ok ? "success" : "failed") << std::endl;

        bool signed_ok = sodium_hex2bin(their_signed_public, crypto_sign_PUBLICKEYBYTES,
                                       their_signed_public_hex.c_str(), their_signed_public_hex.length(),
                                       nullptr, nullptr, nullptr) == 0;
        std::cout << "Signed prekey conversion: " << (signed_ok ? "success" : "failed") << std::endl;

        bool signature_ok = sodium_hex2bin(their_signed_signature, crypto_sign_BYTES,
                                          their_signed_signature_hex.c_str(), their_signed_signature_hex.length(),
                                          nullptr, nullptr, nullptr) == 0;
        std::cout << "Signature conversion: " << (signature_ok ? "success" : "failed") << std::endl;

        if (!device_ok || !identity_ok || !onetime_ok || !signed_ok || !signature_ok) {
            // Clean up on error
            delete[] their_device_public;
            delete[] their_identity_public;
            delete[] their_onetime_public;
            delete[] their_signed_public;
            delete[] their_signed_signature;
            throw std::runtime_error("Failed to decode key bundle data");
        }

        unsigned char *pk_eph = new unsigned char[crypto_sign_BYTES];
        auto sk_buffer_eph = SecureMemoryBuffer::create(ENC_SECRET_KEY_LEN);
        crypto_box_keypair(pk_eph, sk_buffer_eph->data());

        // Create a new KeyBundle
        auto *key_bundle = new SendingKeyBundle(
            reinterpret_cast<unsigned char*>(const_cast<char*>(pk_device.constData())),
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
    unsigned char *their_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
    if (sodium_hex2bin(their_identity_public, crypto_sign_PUBLICKEYBYTES,
                      their_identity_public_hex.c_str(), their_identity_public_hex.length(),
                      nullptr, nullptr, nullptr) != 0) {
        delete[] their_identity_public;
        throw std::runtime_error("Failed to decode their identity public key");
    }

    // Update or create identity session
    IdentityManager::getInstance().update_or_create_identity_sessions(bundles, username, SessionTokenManager::instance().getUsername());
}

void post_handshake_device(
    const unsigned char *identity_session_id,
    const unsigned char *recipient_device_key_public,
    const unsigned char *recipient_signed_prekey_public,
    const unsigned char *recipient_signed_prekey_signature,
    const unsigned char *recipient_onetime_prekey_public,
    const unsigned char *my_device_key_public,
    const unsigned char *my_ephemeral_key_public
) {
    json body = {
        {"identity_session_id", sodium_bin2hex(new char[crypto_hash_sha256_BYTES * 2 + 1], crypto_hash_sha256_BYTES * 2 + 1, identity_session_id, crypto_hash_sha256_BYTES)},
        {"recipient_device_key", sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, recipient_device_key_public, crypto_box_PUBLICKEYBYTES)},
        {"recipient_signed_public_prekey", sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, recipient_signed_prekey_public, crypto_box_PUBLICKEYBYTES)},
        {
            "recipient_signed_public_prekey_signature",
            sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, recipient_signed_prekey_signature, crypto_box_PUBLICKEYBYTES)
        },
        {"recipient_onetime_public_prekey", sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, recipient_onetime_prekey_public, crypto_box_PUBLICKEYBYTES)},
        {"initiator_ephemeral_public_key", sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, my_ephemeral_key_public, crypto_box_PUBLICKEYBYTES)},
        {"initiator_device_public_key", sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, my_device_key_public, crypto_box_PUBLICKEYBYTES)},
    };
    post(body, "/handshake");
}

std::tuple<std::vector<KeyBundle*>, unsigned char*> get_handshake_backlog() {
    json response = get("/incomingHandshakes");
    std::cout << "Raw response: " << response.dump() << std::endl;
    std::cout << "Response keys: ";
    for (auto& [key, value] : response.items()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    std::vector<KeyBundle*> bundles;
    auto identity_session_id = new unsigned char[crypto_hash_sha256_BYTES];  // 32 bytes for final hash

    for (const auto& handshake : response["data"]) {
        auto initator_dev_key = new unsigned char[crypto_box_PUBLICKEYBYTES];
        auto initiator_eph_pub = new unsigned char[crypto_box_PUBLICKEYBYTES];
        auto recip_onetime_pub = new unsigned char[crypto_box_PUBLICKEYBYTES];

        std::string dev_key_str = handshake["initiator_device_public_key"].get<std::string>();
        std::string eph_pub_str = handshake["initiator_ephemeral_public_key"].get<std::string>();
        std::string onetime_pub_str = handshake["recipient_onetime_public_prekey"].get<std::string>();
        std::string session_id_str = handshake["identity_session_id"].get<std::string>();

        std::cout << "Session ID hex length: " << session_id_str.length() << std::endl;
        std::cout << "Expected binary size: " << crypto_hash_sha256_BYTES << std::endl;
        std::cout << "Session ID hex: " << session_id_str << std::endl;

        // First decode the session ID using sodium_hex2bin
        size_t bin_len;
        if (sodium_hex2bin(identity_session_id, crypto_hash_sha256_BYTES,
                          session_id_str.c_str(), session_id_str.length(),
                          nullptr, &bin_len, nullptr) != 0 || bin_len != crypto_hash_sha256_BYTES) {
            delete[] initator_dev_key;
            delete[] initiator_eph_pub;
            delete[] recip_onetime_pub;
            delete[] identity_session_id;
            throw std::runtime_error("Failed to decode session ID");
        }

        bool success = sodium_hex2bin(initator_dev_key, crypto_box_PUBLICKEYBYTES,
                                     dev_key_str.c_str(), dev_key_str.length(),
                                     nullptr, nullptr, nullptr) == 0 &&
            sodium_hex2bin(initiator_eph_pub, crypto_box_PUBLICKEYBYTES,
                          eph_pub_str.c_str(), eph_pub_str.length(),
                          nullptr, nullptr, nullptr) == 0 &&
            sodium_hex2bin(recip_onetime_pub, crypto_box_PUBLICKEYBYTES,
                          onetime_pub_str.c_str(), onetime_pub_str.length(),
                          nullptr, nullptr, nullptr) == 0;

        if (!success) {
            std::cout << "Failed to decode hex strings:" << std::endl;
            std::cout << "Device key hex length: " << dev_key_str.length() << std::endl;
            std::cout << "Ephemeral key hex length: " << eph_pub_str.length() << std::endl;
            std::cout << "One-time key hex length: " << onetime_pub_str.length() << std::endl;
            std::cout << "Session ID hex length: " << session_id_str.length() << std::endl;
            delete[] initator_dev_key;
            delete[] initiator_eph_pub;
            delete[] recip_onetime_pub;
            delete[] identity_session_id;
            throw std::runtime_error("Failed to decode handshake backlog data");
        }

        auto device_key = get_public_key("device");
        auto new_bundle = new ReceivingKeyBundle(
            initator_dev_key,
            initiator_eph_pub,
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(device_key.constData())),
            recip_onetime_pub
        );

        bundles.push_back(new_bundle);
    }

    return std::make_tuple(bundles, identity_session_id);
}

void post_new_keybundles(std::tuple<QByteArray, std::unique_ptr<SecureMemoryBuffer> > device_keypair,
                         std::tuple<unsigned char *, std::unique_ptr<SecureMemoryBuffer> > signed_prekeypair,
                         std::vector<std::tuple<unsigned char *, std::unique_ptr<SecureMemoryBuffer>,
                             unsigned char *> > otks) {
    auto [pk_signed, sk_signed] = std::move(signed_prekeypair);
    auto [pk_device_q, sk_device] = std::move(device_keypair);
    auto pk_device = reinterpret_cast<const unsigned char*>(pk_device_q.constData());

    //sign the public key with device key
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, pk_signed, crypto_sign_PUBLICKEYBYTES, sk_device->data());

    // Convert signature to hex string
    std::string signature_hex = sodium_bin2hex(new char[crypto_sign_BYTES * 2 + 1], crypto_sign_BYTES * 2 + 1, signature, crypto_sign_BYTES);
    std::string signed_prekey_pub_hex = sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, pk_signed, crypto_box_PUBLICKEYBYTES);

    // Create JSON payload
    json body = {
        {"signedpre_key", signed_prekey_pub_hex},
        {"signedpk_signature", signature_hex},
        {"one_time_keys", json::array()}
    };

    for (const auto &[pk, sk, nonce]: otks) {
        body["one_time_keys"].push_back(sodium_bin2hex(new char[crypto_box_PUBLICKEYBYTES * 2 + 1], crypto_box_PUBLICKEYBYTES * 2 + 1, pk, crypto_box_PUBLICKEYBYTES));
    }
    post(body, "/updateKeybundle");
}
