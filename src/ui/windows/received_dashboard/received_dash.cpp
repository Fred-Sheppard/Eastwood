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

    // get any ratchets im missing
    auto handshake_backlog = get_handshake_backlog();
    std::unordered_map<std::string, std::vector<KeyBundle*>> grouped_unordered;

    for (const auto& [username, keybundle] : handshake_backlog) {
        grouped_unordered[username].push_back(keybundle);
    }

    for (auto [username, keybundles] : grouped_unordered) {
        RatchetSessionManager::instance().create_ratchets_if_needed(username, keybundles);
    }

    // get any messages
    auto messages = get_messages();

    for (auto [username, msg] : messages) {
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
        
        save_message_and_key(username, msg->header->device_id, file_uuid, encrypted_message_again, message_nonce, encrypted_key, key_nonce);
        
        // Clean up allocated arrays
        delete[] message_nonce;
        delete[] key_nonce;
    }

    refreshFileList();
}

Received::~Received()
{
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
    auto* item = new QListWidgetItem(ui->fileList);
    auto* widget = new FileItemWidget(fileName, fileSize, timestamp, owner, uuid,
                                    FileItemWidget::Mode::Received, this);

    connect(widget, &FileItemWidget::fileClicked, this, &Received::onFileItemClicked);
    connect(widget, &FileItemWidget::downloadFileClicked, this, &Received::onDownloadFileClicked);

    item->setSizeHint(widget->sizeHint());
    ui->fileList->addItem(item);
    ui->fileList->setItemWidget(item, widget);
}

void Received::refreshFileList()
{
    ui->fileList->clear();

    auto pre_existing_messages = get_all_decrypted_messages();
    // TODO: Fetch actual files from server
    // Example data for demonstration
    for (auto [username, file_uuid, device_id, decrypted_message] : pre_existing_messages) {
        addFileItem(QString::fromStdString(file_uuid), bin2hex(decrypted_message.data(), sizeof(decrypted_message)).data(), "2024-03-15 14:30", bin2hex(device_id.data(),32).data(), QString::fromStdString(file_uuid));
    }
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

}