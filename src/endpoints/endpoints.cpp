#include "endpoints.h"
#include <nlohmann/json.hpp>

#include "src/key_exchange/utils.h"

using json = nlohmann::json;

void post_register_user(
    const std::string &username,
    unsigned char pk_identity[crypto_sign_PUBLICKEYBYTES],
    unsigned char registration_nonce[NONCE_LEN],
    unsigned char nonce_signature[crypto_sign_BYTES]
) {
    json body = {
        {"pk_identity", bin2hex(pk_identity, crypto_sign_PUBLICKEYBYTES)},
        {"registration_nonce", bin2hex(registration_nonce, crypto_sign_BYTES)},
        {"nonce_signature", bin2hex(nonce_signature, crypto_sign_BYTES)}
    };
    // TODO:  make_unauthorised_request_post("/registerUser", body.dump());
};

void post_register_device(
    unsigned char pk_device[crypto_sign_PUBLICKEYBYTES],
    unsigned char pk_signature[crypto_sign_BYTES]
) {
    json body = {
        {"pk_device", bin2hex(pk_device, crypto_sign_PUBLICKEYBYTES)},
        {"pk_signature", bin2hex(pk_signature, crypto_sign_BYTES)}
    };
    // TODO:  make_auth_request_post("/registerDevice", body.dump());
};
