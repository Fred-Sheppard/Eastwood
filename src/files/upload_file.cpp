#include "upload_file.h"

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <vector>
#include <nlohmann/json.hpp>

#include "src/algorithms/algorithms.h"
#include "src/endpoints/endpoints.h"

std::string upload_file(const std::string &file_path) {
    QFile file(file_path.c_str());

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("File " + file_path + " not found");
    }

    // Get file metadata
    const QFileInfo fileInfo(file_path.c_str());
    const QMimeDatabase mimeDb;
    const QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo);

    const QByteArray buff = file.readAll();
    file.close();

    const json metadata = {
        {"name", fileInfo.fileName().toStdString()},
        {"size", fileInfo.size()},
        {"mime_type", mimeType.name().toStdString()}
    };
    const QByteArray metadataBytes = QByteArray::fromStdString(metadata.dump());

    const auto file_key = SecureMemoryBuffer::create(SYM_KEY_LEN);
    randombytes_buf(file_key->data(), SYM_KEY_LEN);
    const auto metadata_key = SecureMemoryBuffer::create(SYM_KEY_LEN);
    memcpy(metadata_key->data(), file_key->data(), SYM_KEY_LEN);

    unsigned char nonce[CHA_CHA_NONCE_LEN];
    randombytes_buf(nonce, CHA_CHA_NONCE_LEN);

    const auto encrypted_file_data = encrypt_bytes(buff, std::move(file_key), nonce);
    const auto encrypted_metadata = encrypt_bytes(metadataBytes, std::move(metadata_key), nonce);

    const std::string file_uuid = post_upload_file(encrypted_file_data, encrypted_metadata);
    return file_uuid;
}
