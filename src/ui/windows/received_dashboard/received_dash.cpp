#include "received_dash.h"
#include "ui_received_dash.h"
#include "src/ui/utils/messagebox.h"
#include "src/ui/utils/window_manager/window_manager.h"
#include "src/ui/utils/navbar/navbar.h"
#include "src/ui/windows/sent_dashboard/sent_dash.h"
#include <QFileDialog>
#include <QTimer>
#include <QCheckBox>
#include "src/auth/logout.h"
#include "src/endpoints/endpoints.h"
#include "src/key_exchange/XChaCha20-Poly1305.h"
#include "src/sessions/RatchetSessionManager.h"
#include <iostream>
#include <ctime>
#include <nlohmann/json_fwd.hpp>

#include "src/client_api_interactions/MakeAuthReq.h"

Received::Received(QWidget *parent, QWidget* sendFileWindow)
    : QWidget(parent)
    , ui(new Ui::Received)
    , m_sendFileWindow(sendFileWindow)
{
    ui->setupUi(this);
    setupConnections();
    setupFileList();
    
    // Connect WindowManager signal to handle navbar highlighting
    connect(&WindowManager::instance(), &WindowManager::windowShown,
            this, &Received::onWindowShown);

    // Use QTimer to defer heavy initialization until after the widget is fully constructed
    QTimer::singleShot(0, this, &Received::initializeData);
}

Received::~Received()
{
    // Clean up all QListWidgetItem objects and their custom widgets before destroying the UI
    clearFileList();
    delete ui;
}

void Received::setupConnections()
{
    connect(ui->sendButton, &QPushButton::clicked, this, &Received::onSendButtonClicked);
    
    // Connect NavBar signals
    NavBar* navbar = findChild<NavBar*>();
    if (navbar) {
        connect(navbar, &NavBar::sentClicked, this, &Received::onSentButtonClicked);
        connect(navbar, &NavBar::sendFileClicked, this, &Received::onSendFileButtonClicked);
        connect(navbar, &NavBar::settingsClicked, this, &Received::onSettingsButtonClicked);
        connect(navbar, &NavBar::logoutClicked, this, &Received::onLogoutButtonClicked);
    }
}

void Received::setupFileList()
{
    ui->fileList->setSpacing(2);
    ui->fileList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileList->setSelectionMode(QAbstractItemView::NoSelection);
}

void Received::addFileItem(const QString& fileName,
                         const QString& fileSize,
                         const QString& timestamp,
                         const QString& owner,
                         const QString& uuid)
{
    // Create the custom widget first
    auto* widget = new FileItemWidget(fileName, fileSize, timestamp, owner, uuid,
                                    FileItemWidget::Mode::Received, this);

    connect(widget, &FileItemWidget::fileClicked, this, &Received::onFileItemClicked);
    connect(widget, &FileItemWidget::downloadFileClicked, this, &Received::onDownloadFileClicked);

    // Create the list item and let QListWidget manage its memory
    auto* item = new QListWidgetItem();
    item->setSizeHint(widget->sizeHint());
    
    // Add the item to the list first, then set the widget
    ui->fileList->addItem(item);
    ui->fileList->setItemWidget(item, widget);
}

void Received::refreshFileList()
{
    clearFileList();

    auto pre_existing_messages = get_all_decrypted_messages();
    std::vector<std::string> uuids;
    for (auto [username, file_uuid, device_id, decrypted_message] : pre_existing_messages) {
        uuids.push_back(file_uuid);
    }

    //auto get_files_metadata = get_encrypted_file_metadata(uuids);

    // TODO: Fetch actual files from server
    // Example data for demonstration
    for (auto [username, file_uuid, device_id, decrypted_message] : pre_existing_messages) {
        // auto decrypted_metadata = decrypt_message_given_key(metadata.data(), metadata.size(), get_decrypted_message(file_uuid).data()); //add
        // json decrypted_metadata_json = json::parse(decrypted_metadata);
        addFileItem(QString::fromStdString(file_uuid), "Unknown size", "2024-03-15 14:30", QString::fromStdString(username), QString::fromStdString(file_uuid));
    }
}

void Received::clearFileList()
{
    if (!ui || !ui->fileList) {
        return; // Safety check in case UI is already destroyed
    }
    
    // Iterate through all items in reverse order to avoid index issues
    while (ui->fileList->count() > 0) {
        QListWidgetItem* item = ui->fileList->takeItem(0);  // Remove and take ownership
        if (item) {
            // Get and clean up the custom widget
            QWidget* widget = ui->fileList->itemWidget(item);
            if (widget) {
                // Cast to FileItemWidget to ensure proper cleanup
                FileItemWidget* fileWidget = qobject_cast<FileItemWidget*>(widget);
                if (fileWidget) {
                    // Disconnect all signals to prevent dangling connections
                    fileWidget->disconnect();
                }
                delete widget;  // Delete the custom widget
            }
            delete item;  // Explicitly delete the QListWidgetItem
        }
    }
    
    // Final clear to ensure the list widget is clean
    ui->fileList->clear();
}

void Received::navigateTo(QWidget* newWindow)
{
    newWindow->setParent(this->parentWidget());  // Set the same parent
    newWindow->show();
    this->setAttribute(Qt::WA_DeleteOnClose);  // Mark for deletion when closed
    close();  // This will trigger deletion due to WA_DeleteOnClose
}

void Received::onSendButtonClicked()
{
    WindowManager::instance().showSendFile();
}

void Received::onFileItemClicked(FileItemWidget* widget)
{
    showFileMetadata(widget);
}

void Received::onSentButtonClicked()
{
    WindowManager::instance().showSent();
}

void Received::onSettingsButtonClicked()
{
    WindowManager::instance().showSettings(); 
}

void Received::showFileMetadata(FileItemWidget* widget)
{
    StyledMessageBox::info(this, "File Details",
                       QString("File Details:\n\nName: %1\nSize: %2\nShared by: %3\nTimestamp: %4")
                       .arg(widget->getFileName())
                       .arg(widget->getFileSize())
                       .arg(widget->getOwner())
                       .arg(widget->getTimestamp()));
}

void Received::sendFileToUser(const QString& username, const QString& fileId)
{
    // TODO: Implement file sharing logic
}

// navbar button
void Received::onSendFileButtonClicked()
{
    WindowManager::instance().showSendFile();
}

void Received::onWindowShown(const QString& windowName)
{
    // Find the navbar and update its active button
    NavBar* navbar = findChild<NavBar*>();
    if (navbar) {
        navbar->setActiveButton(windowName);
    }
}

void Received::onLogoutButtonClicked()
{
    logout();
    WindowManager::instance().showLogin();
}

void Received::onDownloadFileClicked(FileItemWidget* widget)
{
    std::string uuid = widget->getUuid().toStdString();
    std::cout << "Downloading file with UUID: " << uuid << std::endl;
    
    auto encrypted_file = get_encrypted_file(uuid);
    if (encrypted_file.empty()) {
        std::cout << "ERROR: get_encrypted_file returned empty data for UUID: " << uuid << std::endl;
        return;
    }
    
    std::cout << "Retrieved encrypted file, size: " << encrypted_file.size() << " bytes" << std::endl;
    
    auto decrypted_message = get_decrypted_message(uuid);
    if (decrypted_message.empty()) {
        std::cout << "ERROR: get_decrypted_message returned empty data for UUID: " << uuid << std::endl;
        return;
    }
    
    std::cout << "Retrieved decrypted message, size: " << decrypted_message.size() << " bytes" << std::endl;
    
    // Debug: Print full hex dump of decrypted message
    std::cout << "Decrypted message hex dump:" << std::endl;
    for (size_t i = 0; i < decrypted_message.size(); i++) {
        if (i % 16 == 0) std::cout << std::endl << std::hex << std::setfill('0') << std::setw(4) << i << ": ";
        printf("%02x ", decrypted_message[i]);
    }
    std::cout << std::dec << std::endl << std::endl;
    
    // Debug: Try to interpret as string to see if it's JSON or text
    std::string decrypted_as_string(decrypted_message.begin(), decrypted_message.end());
    std::cout << "Decrypted message as string: '" << decrypted_as_string << "'" << std::endl;
    
    // Check if it looks like JSON
    if (decrypted_as_string.front() == '{' && decrypted_as_string.back() == '}') {
        std::cout << "Decrypted message appears to be JSON format" << std::endl;
        // TODO: Parse JSON to extract the key
    } else {
        std::cout << "Decrypted message does not appear to be JSON" << std::endl;
    }
    
    // The decrypted message should contain the file encryption key
    // Based on the hex dump, the message is 64 bytes with the key in the second half
    if (decrypted_message.size() < 64) {
        std::cout << "ERROR: Decrypted message too small. Expected 64 bytes, got: " << decrypted_message.size() << std::endl;
        return;
    }
    
    // Looking at the hex dump, the actual key data appears to be 16 bytes at offset 32
    // Extract bytes 32-48 (16 bytes of actual key data)
    std::vector<unsigned char> file_encryption_key_16(decrypted_message.begin() + 32, decrypted_message.begin() + 48);
    
    // Extend to 32 bytes if needed (ChaCha20-Poly1305 typically uses 32-byte keys)
    std::vector<unsigned char> file_encryption_key;
    if (file_encryption_key_16.size() == 16) {
        // If we only have 16 bytes, we might need to derive a 32-byte key
        // For now, let's try doubling the 16-byte key
        file_encryption_key.reserve(32);
        file_encryption_key.insert(file_encryption_key.end(), file_encryption_key_16.begin(), file_encryption_key_16.end());
        file_encryption_key.insert(file_encryption_key.end(), file_encryption_key_16.begin(), file_encryption_key_16.end());
    } else {
        file_encryption_key = file_encryption_key_16;
    }
    
    std::cout << "Extracted file encryption key from bytes 32-48, original size: " << file_encryption_key_16.size() << " bytes" << std::endl;
    std::cout << "Extended key size: " << file_encryption_key.size() << " bytes" << std::endl;
    std::cout << "Key first 20 bytes: ";
    for (size_t i = 0; i < std::min((size_t)20, file_encryption_key.size()); i++) {
        printf("%02x ", file_encryption_key[i]);
    }
    std::cout << std::endl;

    try {
        auto decrypted_file = decrypt_message_given_key(encrypted_file.data(), encrypted_file.size(), file_encryption_key.data());
        std::cout << "Successfully decrypted file, size: " << decrypted_file.size() << " bytes" << std::endl;
        // save to laptop idek
    } catch (const std::exception& e) {
        std::cout << "ERROR: Failed to decrypt file: " << e.what() << std::endl;
    }
}

void Received::initializeData()
{
    try {
        // get any ratchets im missing
        auto handshake_backlog = get_handshake_backlog();
        std::unordered_map<std::string, std::vector<KeyBundle*>> grouped_unordered;

        for (const auto& [username, keybundle] : handshake_backlog) {
            grouped_unordered[username].push_back(keybundle);
        }

        for (auto [username, keybundles] : grouped_unordered) {
            RatchetSessionManager::instance().create_ratchets_if_needed(username, keybundles);
        }

        // Clean up KeyBundle objects from handshake_backlog
        for (const auto& [username, keybundle] : handshake_backlog) {
            delete keybundle;
        }

        // get any messages
        auto messages = get_messages();
        
        std::cout << "Processing " << messages.size() << " new messages..." << std::endl;

        for (auto [username, msg] : messages) {
            try {
                auto key = RatchetSessionManager::instance().get_key_for_device(username, msg->header);

                auto decrypted_message = decrypt_message_given_key(msg->ciphertext, msg->length, key);

                // encrypt again and save to db
                auto message_encryption_key = SecureMemoryBuffer::create(32);
                crypto_aead_chacha20poly1305_ietf_keygen(message_encryption_key->data());

                // Generate nonce for message encryption
                auto message_nonce = new unsigned char[CHA_CHA_NONCE_LEN];
                randombytes_buf(message_nonce, CHA_CHA_NONCE_LEN);

                // Copy the encryption key for saving BEFORE moving it
                auto sk_buffer = SecureMemoryBuffer::create(32);
                memcpy(sk_buffer->data(), message_encryption_key->data(), 32);

                auto encrypted_message_again = encrypt_bytes(
                    QByteArray(reinterpret_cast<const char*>(decrypted_message.data()), decrypted_message.size()), 
                    std::move(message_encryption_key), 
                    message_nonce
                );

                auto key_nonce = new unsigned char[CHA_CHA_NONCE_LEN];
                randombytes_buf(key_nonce, CHA_CHA_NONCE_LEN);

                auto encrypted_key = encrypt_symmetric_key(sk_buffer, key_nonce);
                
                // Extract file_uuid from header (convert char array to string)
                std::string file_uuid(msg->header->file_uuid);
                
                // Debug output
                std::cout << "Processing message from user: " << username << std::endl;
                std::cout << "file_uuid length: " << file_uuid.length() << std::endl;
                std::cout << "file_uuid content: '" << file_uuid << "'" << std::endl;
                if (file_uuid.empty()) {
                    std::cout << "WARNING: file_uuid is empty! Using placeholder." << std::endl;
                    file_uuid = "unknown_file_" + std::to_string(std::time(nullptr));
                }
                
                // Save to database
                try {
                    save_message_and_key(username, msg->header->device_id, file_uuid, encrypted_message_again, message_nonce, encrypted_key, key_nonce);
                    std::cout << "Successfully saved message for file_uuid: " << file_uuid << std::endl;
                } catch (const std::exception& save_error) {
                    std::cout << "ERROR: Failed to save message for file_uuid: " << file_uuid << " - " << save_error.what() << std::endl;
                }
                
                // Clean up allocated arrays
                delete[] message_nonce;
                delete[] key_nonce;
                
            } catch (const std::exception& e) {
                std::cout << "ERROR: Failed to process message from " << username << ": " << e.what() << std::endl;
            }
            
            // Clean up the DeviceMessage and its components - CRITICAL for preventing memory leaks
            delete[] msg->ciphertext;  // Clean up ciphertext array
            delete msg->header;        // Clean up MessageHeader
            delete msg;                // Clean up DeviceMessage
        }

        std::cout << "Finished processing messages. Refreshing file list..." << std::endl;
        
        // Use QTimer to defer the refresh to ensure database operations are fully committed
        // Also refresh immediately to show any existing messages
        refreshFileList();
        
        // And schedule another refresh after a short delay to catch any newly saved messages
        if (!messages.empty()) {
            QTimer::singleShot(200, this, [this]() {
                std::cout << "Performing delayed refresh to ensure all database operations are visible..." << std::endl;
                refreshFileList();
            });
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error during initialization: " << e.what() << std::endl;
        // Still try to refresh file list to show any existing data
        refreshFileList();
    }
}