#ifndef CONVERSION_UTILS_H
#define CONVERSION_UTILS_H

#include <qbytearray.h>
#include <vector>

bool hex_to_bin(const std::string& hex, unsigned char* bin, size_t bin_size);
std::string bin_to_hex(const unsigned char* bin, size_t bin_size);
std::vector<unsigned char> hex_string_to_binary(const std::string& hex_string);

// Load environment variable from .env file
std::string load_env_variable(const std::string& key, const std::string& default_value = "");

const unsigned char *q_byte_array_to_chars(const QByteArray &qb);

#endif // CONVERSION_UTILS_H