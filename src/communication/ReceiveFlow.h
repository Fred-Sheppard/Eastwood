//
// Created by Josh Sloggett on 04/06/2025.
//

#ifndef RECEIVEFLOW_H
#define RECEIVEFLOW_H
#include <nlohmann/detail/input/parser.hpp>

#include "src/client_api_interactions/MakeAuthReq.h"
#include "src/endpoints/endpoints.h"
#include "src/key_exchange/XChaCha20-Poly1305.h"
#include "src/sessions/RatchetSessionManager.h"

inline void update_handshakes() {
    auto handshakes = get_handshake_backlog();

    for (auto &[username, keybundle] : handshakes) {
        RatchetSessionManager::instance().create_ratchets_if_needed(username, {keybundle});
    }
}

inline void update_messages() {
    auto messages = get_messages();

    for (auto &[username, message] : messages) {
        auto key = RatchetSessionManager::instance().get_key_for_device(username, message.header);
        auto decrypted_message = decrypt_message_given_key(message.ciphertext.data(), message.ciphertext.size(), key.data());
        auto q_decrypted_message = reinterpret_cast<const char*>(decrypted_message.data(), static_cast<int>(decrypted_message.size()));

        // re encrypt with new key
        std::unique_ptr<SecureMemoryBuffer> new_db_message_key = SecureMemoryBuffer::create(32);
        crypto_stream_xchacha20_keygen(new_db_message_key->data());

        std::array<unsigned char, CHA_CHA_NONCE_LEN> nonce_for_msg{};
        randombytes_buf(nonce_for_msg.data(), CHA_CHA_NONCE_LEN);

        auto encrypted_message_for_db = encrypt_bytes(q_decrypted_message, new_db_message_key, nonce_for_msg.data());

        std::array<unsigned char, CHA_CHA_NONCE_LEN> nonce_for_key{};
        randombytes_buf(nonce_for_key.data(), CHA_CHA_NONCE_LEN);
        auto encrypted_key = encrypt_symmetric_key(new_db_message_key, nonce_for_key.data());

        // save to db
        save_message_and_key(username, message.header.device_id, message.header.file_uuid, encrypted_message_for_db, nonce_for_msg.data(), encrypted_key, nonce_for_key.data());
    }
}

// vector of file name, file size, mime type
inline std::vector<std::tuple<std::string, int, std::string>> get_file_metadata() {
    auto uuids = get_all_received_file_uuids();
    auto encrypted_metadata = get_encrypted_file_metadata(uuids);

    std::vector<std::tuple<std::string, int, std::string>> file_metadata;
    for (auto &[uuid, ciphertext] : encrypted_metadata) {
        auto key = get_decrypted_message(uuid);
        auto metadata = json::parse(decrypt_message_given_key(ciphertext.data(), ciphertext.size(), key.data()));\

        file_metadata.emplace_back(std::make_tuple(metadata["name"].get<std::string>(), metadata["size"].get<int>(), metadata["mime_type"].get<std::string>()));
    }

    return file_metadata;
}

#endif //RECEIVEFLOW_H
