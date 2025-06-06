#include "login.h"

#include "src/keys/kek_manager.h"
#include "src/auth/salt.h"
#include "src/endpoints/endpoints.h"
#include "src/keys/session_token_manager.h"
#include "src/key_exchange/utils.h"
#include "src/sessions/RatchetSessionManager.h"
#include "src/sql/queries.h"

void login_user(const std::string &username, std::unique_ptr<SecureMemoryBuffer>&& master_password, bool post_new_keys) {
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

    RatchetSessionManager::instance().load_ratchets_from_db();

    if (post_new_keys) {
        std::tuple<unsigned char *, std::unique_ptr<SecureMemoryBuffer> > prekey;

        const QDateTime lastUpdated = get_signed_prekey_last_updated();
        if (!lastUpdated.isValid() || lastUpdated < QDateTime::currentDateTime().addDays(-14)) {
            prekey = generate_signed_prekey();
        }

        post_new_keybundles(get_decrypted_keypair("device"), &prekey, generate_onetime_keys(100));
    }
}
