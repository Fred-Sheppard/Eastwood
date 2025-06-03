#include "upload_file.h"

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <vector>
#include <nlohmann/json.hpp>

#include "src/algorithms/algorithms.h"
#include "src/endpoints/endpoints.h"
#include "src/key_exchange/XChaCha20-Poly1305.h"

std::tuple<std::string, std::unique_ptr<SecureMemoryBuffer> > upload_file(const std::string &file_path) {
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

    const auto file_key = SecureMemoryBuffer::create(SYM_KEY_LEN);
    randombytes_buf(file_key->data(), SYM_KEY_LEN);
    const auto metadata_key = SecureMemoryBuffer::create(SYM_KEY_LEN);
    memcpy(metadata_key->data(), file_key->data(), SYM_KEY_LEN);

    unsigned char nonce[CHA_CHA_NONCE_LEN];
    randombytes_buf(nonce, CHA_CHA_NONCE_LEN);

    // qDebug() << "Raw buffer" << bin2hex(reinterpret_cast<const unsigned char *>(buff.data()), buff.size());
    // const auto encrypted_file_data = encrypt_bytes(buff, std::move(file_key), nonce);
    //
    // const std::vector nonce_vec(nonce, nonce + CHA_CHA_NONCE_LEN);
    // const auto decrypted = decrypt_bytes(buff, std::move(file_key), nonce_vec);
    // qDebug() << "Decrypted file" << bin2hex(decrypted.data(), decrypted.size());

    qDebug() << "Raw buffer" << bin2hex(reinterpret_cast<const unsigned char *>(buff.data()), buff.size());
    const auto encrypted_file_data = encrypt_message_given_key(reinterpret_cast<const unsigned char *>(buff.data()), buff.size(), file_key->data());

    const auto decrypted_file_data = decrypt_message_given_key(encrypted_file_data.data(), encrypted_file_data.size(), file_key->data());
    qDebug() << "Decrypted file" << bin2hex(decrypted_file_data.data(), decrypted_file_data.size());


    const json metadata = {
        {"name", fileInfo.fileName().toStdString()},
        {"size", fileInfo.size()},
        {"mime_type", mimeType.name().toStdString()},
    };
    const QByteArray metadataBytes = QByteArray::fromStdString(metadata.dump());

    const auto encrypted_metadata = encrypt_bytes(metadataBytes, std::move(metadata_key), nonce);


    const std::string file_uuid = post_upload_file(encrypted_file_data, encrypted_metadata);

    auto output_key = SecureMemoryBuffer::create(SYM_KEY_LEN);
    memcpy(metadata_key->data(), file_key->data(), SYM_KEY_LEN);

    return std::make_tuple(file_uuid, std::move(output_key));
}
