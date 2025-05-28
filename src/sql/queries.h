//
// Created by Fred Sheppard on 27/05/2025.
//

#ifndef QUERIES_H
#define QUERIES_H

#include <tuple>
#include "src/algorithms/constants.h"
#include "src/database/database.h"
#include <QByteArray>
#include <string>

std::tuple<QByteArray, QByteArray> get_keypair(const std::string &label);

void save_keypair(
    const std::string &label,
    unsigned char pk_identity[crypto_sign_PUBLICKEYBYTES],
    unsigned char encrypted_sk[crypto_sign_SECRETKEYBYTES + ENC_OVERHEAD],
    unsigned char nonce_sk[NONCE_LEN]
);

void save_encrypted_key(
    const std::string &label,
    unsigned char encrypted_key[crypto_sign_SECRETKEYBYTES + ENC_OVERHEAD],
    unsigned char nonce_sk[NONCE_LEN]
);

#endif //QUERIES_H
