#include <string>

#include <sodium.h>

#include "../salt.h"
#include "src/algorithms/algorithms.h"
#include "src/database/database.h"

void rotate_master_password(const std::string &username, const std::string &new_password) {
    unsigned char new_salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(new_salt, crypto_pwhash_SALTBYTES);

    const auto new_key = derive_master_key(std::make_unique<std::string>(new_password), new_salt);
    Database::get().rotate_master_password(new_key);
    save_salt_to_file(username, new_salt);
}
