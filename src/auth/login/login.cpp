#include "login.h"

#include "src/keys/kek_manager.h"
#include "src/auth/salt.h"
#include "src/endpoints/endpoints.h"
#include "src/keys/session_token_manager.h"
#include "src/key_exchange/utils.h"
#include "src/key_exchange/XChaCha20-Poly1305.h"
#include "src/sessions/RatchetSessionManager.h"
#include "src/sql/queries.h"

void login_user(const std::string &username, const std::unique_ptr<const std::string> &master_password) {
    unsigned char salt[crypto_pwhash_SALTBYTES];
    get_salt_from_file(username, salt);
    const auto master_key = derive_master_key(std::move(master_password), salt);
    Database::get().initialize(username, master_key);

    auto [e_kek, nonce_kek] = get_encrypted_key("kek");
    auto kek = decrypt_kek(e_kek, nonce_kek, std::move(master_key));
    KekManager::instance().setKEK(std::move(kek));

    const auto [pk_device, sk_device] = get_decrypted_keypair("device");

    const auto nonce_login = post_request_login(username, q_byte_array_to_chars(pk_device));
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, nonce_login.data(), nonce_login.size(), sk_device->data());
    const std::string token = post_authenticate(username, q_byte_array_to_chars(pk_device), signature);
    SessionTokenManager::instance().setToken(token);
    SessionTokenManager::instance().setUsername(username);
    post_new_keybundles(
        get_decrypted_keypair("device"),
        generate_signed_prekey(),
        generate_onetime_keys(50)
        );

    auto handshakes = get_handshake_backlog();
    auto messages = get_messages();
    
    // Group handshakes by username and create ratchets for each user
    std::map<std::string, std::vector<KeyBundle*>> handshakes_by_user;
    for (auto [username, bundle] : handshakes) {
        handshakes_by_user[username].push_back(bundle);
    }
    
    // Create ratchets for each user that sent handshakes
    for (auto [username, bundles] : handshakes_by_user) {
        RatchetSessionManager::instance().create_ratchets_if_needed(username, bundles);
        std::cout << "Created ratchets for user: " << username << std::endl;
    }

    // Process each message individually and safely
    for (auto [msg_username, msg] : messages) {
        if (msg == nullptr || msg->header == nullptr) {
            std::cerr << "Null message or header, skipping" << std::endl;
            continue;
        }
        
        try {
            unsigned char* key = RatchetSessionManager::instance().get_key_for_device(msg_username, msg->header);
            if (key != nullptr) {
                auto decrypted = decrypt_message_given_key(msg->ciphertext, msg->length, key);
                std::cout << "Decrypted message: " << bin2hex(decrypted.data(), std::min(32, (int)decrypted.size())) << std::endl;
                delete[] key;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing message: " << e.what() << std::endl;
        }
        
        // Clean up message - DeviceMessage destructor handles header and ciphertext
        delete msg;
    }
    
    // Clean up handshakes
    for (auto [username, bundle] : handshakes) {
        delete bundle;
    }
}
