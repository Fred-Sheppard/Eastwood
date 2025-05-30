#ifndef ENDPOINTS_H
#define ENDPOINTS_H
#include <string>
#include <sodium.h>
#include <array>
#include "src/algorithms/constants.h"
#include "src/keys/secure_memory_buffer.h"
#include "src/key_exchange/DoubleRatchet.h"
#include "src/sessions/IdentityManager.h"

void post_register_user(
    const std::string &username,
    const unsigned char pk_identity[crypto_sign_PUBLICKEYBYTES],
    const unsigned char registration_nonce[CHA_CHA_NONCE_LEN],
    const unsigned char nonce_signature[crypto_sign_BYTES]
);

void post_register_device(
    const unsigned char pk_id[crypto_sign_PUBLICKEYBYTES],
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    const unsigned char pk_signature[crypto_sign_BYTES]
);

std::vector<unsigned char> post_request_login(
    std::string username,
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES]
);

std::string post_authenticate(
    std::string username,
    const unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    unsigned char signature[crypto_sign_BYTES]
);

void post_ratchet_message(
    const DeviceMessage *msg,
    const IdentitySessionId& identity_session_id
);

void post_handshake_device(
    const IdentitySessionId& identity_session_id,
    const unsigned char *recipient_device_key_public,
    const unsigned char *recipient_signed_prekey_public,
    const unsigned char *recipient_signed_prekey_signature,
    const unsigned char *recipient_onetime_prekey_public,
    const unsigned char *my_device_key_public,
    const unsigned char *my_ephemeral_key_public
);

std::vector<std::tuple<IdentitySessionId, KeyBundle*>> get_handshake_backlog();

std::vector<std::tuple<IdentitySessionId, DeviceMessage*>> get_messages();

void get_keybundles(std::string username);

void post_new_keybundles(
    std::tuple<QByteArray, std::unique_ptr<SecureMemoryBuffer>> device_keypair,
    std::tuple<unsigned char*, std::unique_ptr<SecureMemoryBuffer>> signed_prekeypair,
    std::vector<std::tuple<unsigned char*, std::unique_ptr<SecureMemoryBuffer>, unsigned char*>> otks
);

#endif //ENDPOINTS_H
