#ifndef ENCRYPT_FILE_H
#define ENCRYPT_FILE_H
#include <string>
#include <tuple>

#include "src/keys/secure_memory_buffer.h"

std::tuple<std::string, std::unique_ptr<SecureMemoryBuffer>> upload_file(const std::string &file_path);

#endif //ENCRYPT_FILE_H
