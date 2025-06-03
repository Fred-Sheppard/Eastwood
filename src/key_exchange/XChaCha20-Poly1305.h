#ifndef XCHACHA20_POLY1305_H
#define XCHACHA20_POLY1305_H

#include <vector>



std::vector<unsigned char> encrypt_message_given_key(const unsigned char* message, size_t MESSAGE_LEN, const unsigned char* key);
std::vector<unsigned char> encrypt_message_auto_key(const unsigned char* message, const size_t MESSAGE_LEN);
std::vector<unsigned char> decrypt_message_given_key(const unsigned char* encrypted_data, size_t ENCRYPTED_LEN, const unsigned char* key);


#endif // XCHACHA20_POLY1305_H 