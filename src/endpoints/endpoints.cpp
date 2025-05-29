#include "endpoints.h"

#include <nlohmann/json.hpp>

#include "src/key_exchange/utils.h"
#include "src/utils/ConversionUtils.h"
#include "src/client_api_interactions/MakeAuthReq.h"
#include "src/sql/queries.h"
#include "src/client_api_interactions/MakeUnauthReq.h"
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
    std::cout << body << std::endl;

    post_unauth(body, "/registerUser");
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
    post_unauth(body, "/registerDevice");
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
    const json response = post_unauth(body, "/requestLogin");
    QString response_text(response.dump().data());
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
    const json response = post_unauth(body, "/authenticate");
    return response["data"]["token"];
}

void get_messages() {
    // TODO: implement or remove
}

void post_ratchet_message(const DeviceMessage *msg) {
    json body = {
        {"device_id", bin_to_hex(msg->header->device_id, sizeof(msg->header->device_id))},
        {"dh_public", bin_to_hex(msg->header->dh_public, sizeof(msg->header->dh_public))},
        {"prev_chain_length", msg->header->prev_chain_length},
        {"prev_chain_length", msg->header->message_index},
        {"ciphertext", bin_to_hex(msg->ciphertext, sizeof(msg->ciphertext))},
    };
    //todo: post to /sendMessage/deviceId

    post(body, "/sendMessage");
};

void get_keybundles(std::string username) {
    json response = get("/keybundle/" + username);

    // Get my identity public key
    std::string my_identity_public_hex = response["data"]["identity_public_key"];
    unsigned char* my_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
    if (!hex_to_bin(my_identity_public_hex, my_identity_public, crypto_sign_PUBLICKEYBYTES)) {
        delete[] my_identity_public;
        throw std::runtime_error("Failed to decode my identity public key");
    }

    // Get my device and signed prekey private keys from database
    const auto [pk_device, sk_device] = get_decrypted_keypair("device");
    const auto [pk_signed, sk_signed] = get_decrypted_keypair("signed");

    std::vector<KeyBundle*> bundles;
    std::string their_identity_public_hex;

    // Process each key bundle
    for (const auto& bundle : response["data"]["key_bundles"]) {
        // Convert hex strings to binary
        std::string their_device_public_hex = bundle["device_public_key"];
        their_identity_public_hex = bundle["identity_public_key"];
        std::string their_onetime_public_hex = bundle["one_time_key"];
        std::string their_signed_public_hex = bundle["signedpre_key"];
        std::string their_signed_signature_hex = bundle["signedpk_signature"];

        // Allocate memory for binary data
        unsigned char* their_device_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char* their_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char* their_onetime_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char* their_signed_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char* their_signed_signature = new unsigned char[crypto_sign_BYTES];

        // Convert hex to binary
        std::cout << "Converting hex to binary:" << std::endl;
        std::cout << "Device public key hex length: " << their_device_public_hex.length() << ", expected binary size: " << crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "Identity public key hex length: " << their_identity_public_hex.length() << ", expected binary size: " << crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "One-time key hex length: " << their_onetime_public_hex.length() << ", expected binary size: " << crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "Signed prekey hex length: " << their_signed_public_hex.length() << ", expected binary size: " << crypto_sign_PUBLICKEYBYTES << std::endl;
        std::cout << "Signature hex length: " << their_signed_signature_hex.length() << ", expected binary size: " << crypto_sign_BYTES << std::endl;
        
        bool device_ok = hex_to_bin(their_device_public_hex, their_device_public, crypto_sign_PUBLICKEYBYTES);
        std::cout << "Device public key conversion: " << (device_ok ? "success" : "failed") << std::endl;
        
        bool identity_ok = hex_to_bin(their_identity_public_hex, their_identity_public, crypto_sign_PUBLICKEYBYTES);
        std::cout << "Identity public key conversion: " << (identity_ok ? "success" : "failed") << std::endl;
        
        bool onetime_ok = hex_to_bin(their_onetime_public_hex, their_onetime_public, crypto_sign_PUBLICKEYBYTES);
        std::cout << "One-time key conversion: " << (onetime_ok ? "success" : "failed") << std::endl;
        
        bool signed_ok = hex_to_bin(their_signed_public_hex, their_signed_public, crypto_sign_PUBLICKEYBYTES);
        std::cout << "Signed prekey conversion: " << (signed_ok ? "success" : "failed") << std::endl;
        
        bool signature_ok = hex_to_bin(their_signed_signature_hex, their_signed_signature, crypto_sign_BYTES);
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

        unsigned char* pk_eph = new unsigned char[crypto_sign_BYTES];
        unsigned char* sk_eph = new unsigned char[crypto_sign_BYTES];
        crypto_box_keypair(pk_eph, sk_eph);

        // Create a new ReceivingKeyBundle
        auto* key_bundle = new SendingKeyBundle(
            reinterpret_cast<unsigned char*>(const_cast<char*>(pk_device.data())),
            sk_device->data(),
            pk_eph,
            sk_eph,
            their_device_public,
            their_signed_public,
            their_onetime_public,
            their_signed_signature
        );

        bundles.push_back(key_bundle);
    }

    // Convert their identity public key to binary
    unsigned char* their_identity_public = new unsigned char[crypto_sign_PUBLICKEYBYTES];
    if (!hex_to_bin(their_identity_public_hex, their_identity_public, crypto_sign_PUBLICKEYBYTES)) {
        delete[] their_identity_public;
        throw std::runtime_error("Failed to decode their identity public key");
    }

    // Update or create identity session
    IdentityManager::getInstance().update_or_create_identity_sessions(bundles, my_identity_public, their_identity_public);
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
        {"identity_session_id", bin2hex(identity_session_id, crypto_box_PUBLICKEYBYTES * 2)},
        {"recipient_device_key", bin2hex(recipient_device_key_public, crypto_box_PUBLICKEYBYTES)},
        {"recipient_signed_public_prekey", bin2hex(recipient_signed_prekey_public, crypto_box_PUBLICKEYBYTES)},
        {"recipient_signed_public_prekey_signature", bin2hex(recipient_signed_prekey_signature, crypto_box_PUBLICKEYBYTES)},
        {"recipient_onetime_public_prekey", bin2hex(recipient_onetime_prekey_public, crypto_box_PUBLICKEYBYTES)},
        {"initiator_ephemeral_public_key", bin2hex(my_ephemeral_key_public, crypto_box_PUBLICKEYBYTES)}
    };
    post(body, "/handshake");
}

void post_new_keybundles(){
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];

    // Create a secure buffer for the private key
    auto sk_buffer = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
    // Generate new keypair
    crypto_box_keypair(pk, sk_buffer->data());
    
    // Generate nonce for encryption
    unsigned char nonce[CHA_CHA_NONCE_LEN];
    randombytes_buf(nonce, CHA_CHA_NONCE_LEN);
    
    // Encrypt and save the signed prekey pair
    const auto encrypted_sk = encrypt_secret_key(sk_buffer, nonce);
    save_encrypted_keypair("signed", pk, encrypted_sk, nonce);
    
    // Verify signed prekey was saved
    try {
        auto [saved_pk, saved_sk, saved_nonce] = get_encrypted_keypair("signed");
        std::cout << "Successfully saved and retrieved signed prekey" << std::endl;
        std::cout << "Public key length: " << saved_pk.size() << " bytes" << std::endl;
        std::cout << "Encrypted private key length: " << saved_sk.size() << " bytes" << std::endl;
        std::cout << "Nonce length: " << saved_nonce.size() << " bytes" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to retrieve signed prekey: " << e.what() << std::endl;
        throw;
    }
    
    // Convert keys to hex strings
    std::string pk_hex = bin2hex(pk, crypto_sign_PUBLICKEYBYTES);
    std::string sk_hex = bin2hex(sk_buffer->data(), crypto_sign_SECRETKEYBYTES);

    auto decrypted_device_key = get_decrypted_sk("device");
    
    // Convert QByteArray to hex string
    std::string device_key_hex = bin_to_hex(decrypted_device_key->data(), decrypted_device_key->size());
    
    // Convert hex string to bytes
    unsigned char device_key[crypto_sign_SECRETKEYBYTES];
    if (!hex_to_bin(device_key_hex, device_key, crypto_sign_SECRETKEYBYTES)) {
        std::cerr << "Failed to convert device key hex to binary" << std::endl;
        throw;
    }
    
    // Sign the public key with device key
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, pk, crypto_sign_PUBLICKEYBYTES, device_key);
    
    // Convert signature to hex string
    std::string signature_hex = bin2hex(signature, crypto_sign_BYTES);
    
    // Create JSON payload
    json body = {
        {"signedpre_key", pk_hex},
        {"signedpk_signature", signature_hex},
        {"one_time_keys", json::array()}
    };

    std::cout << "Starting one-time key generation..." << std::endl;
    // Vector to store one-time keys
    std::vector<std::tuple<unsigned char*, std::unique_ptr<SecureMemoryBuffer>, unsigned char*>> onetime_keys;

    for (int i = 0; i < 100; i++) {
        try {
            std::cout << "Generating one-time key pair " << i << std::endl;
            
            // Allocate aligned memory for keypair
            unsigned char* onetime_pk = new unsigned char[crypto_box_PUBLICKEYBYTES];
            unsigned char* onetime_sk = new unsigned char[crypto_box_SECRETKEYBYTES];
            
            // Generate new keypair
            if (crypto_box_keypair(onetime_pk, onetime_sk) != 0) {
                delete[] onetime_pk;
                delete[] onetime_sk;
                throw std::runtime_error("Failed to generate one-time keypair");
            }
            std::cout << "Generated keypair " << i << std::endl;
            
            // Convert public key to hex
            std::string onetime_pk_hex = bin2hex(onetime_pk, crypto_box_PUBLICKEYBYTES);
            
            // Add to JSON array
            body["one_time_keys"].push_back(onetime_pk_hex);
            
            // Create a secure buffer for the private key
            auto sk_buffer = SecureMemoryBuffer::create(crypto_box_SECRETKEYBYTES);
            if (!sk_buffer) {
                delete[] onetime_pk;
                delete[] onetime_sk;
                throw std::runtime_error("Failed to create secure buffer");
            }
            std::cout << "Created secure buffer for key " << i << std::endl;
            
            // Copy private key to secure buffer
            std::memcpy(sk_buffer->data(), onetime_sk, crypto_box_SECRETKEYBYTES);
            std::cout << "Copied private key to secure buffer for key " << i << std::endl;
            
            // Clean up the original private key
            delete[] onetime_sk;
            
            // Generate unique nonce for this key
            unsigned char* key_nonce = new unsigned char[CHA_CHA_NONCE_LEN];
            if (!key_nonce) {
                delete[] onetime_pk;
                throw std::runtime_error("Failed to allocate nonce");
            }
            randombytes_buf(key_nonce, CHA_CHA_NONCE_LEN);
            std::cout << "Generated nonce for key " << i << std::endl;
            
            // Encrypt the private key
            std::cout << "Encrypting private key " << i << std::endl;
            auto encrypted_sk = encrypt_secret_key(sk_buffer, key_nonce);
            if (!encrypted_sk) {
                delete[] onetime_pk;
                delete[] key_nonce;
                throw std::runtime_error("Failed to encrypt private key");
            }
            std::cout << "Encrypted private key " << i << " (size: " << encrypted_sk->size() << " bytes)" << std::endl;
            
            // Add to vector of keys
            onetime_keys.emplace_back(onetime_pk, std::move(encrypted_sk), key_nonce);
            std::cout << "Added key " << i << " to vector" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error processing one-time key " << i << ": " << e.what() << std::endl;
            throw;
        }
    }
    
    std::cout << "Generated all one-time keys, saving to database..." << std::endl;
    // Save all one-time keys to database
    save_encrypted_onetime_keys(std::move(onetime_keys));
    
    // Verify one-time keys were saved
    try {
        const auto &db = Database::get();
        sqlite3_stmt *stmt;
        db.prepare_or_throw("SELECT COUNT(*) as count FROM onetime_prekeys", &stmt);
        auto rows = db.query(stmt);
        if (!rows.empty()) {
            std::cout << "Successfully saved " << rows[0]["count"].toInt() << " one-time keys" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to verify one-time keys: " << e.what() << std::endl;
        throw;
    }
    
    // Post to server
    post(body, "/updateKeybundle");
}
