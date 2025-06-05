#include "./device_register.h"
#include "ui_device_register.h"
#include "src/ui/utils/window_manager/window_manager.h"
#include <iostream>
#include <QClipboard>
#include <QApplication>
#include <thread>
#include <unistd.h>
#include <QDebug>
#include <QMetaObject>
#include <QCoreApplication>

#include "src/auth/login/login.h"
#include "src/endpoints/endpoints.h"
#include "src/ui/utils/messagebox.h"
#include "src/auth/set_up_client.h"
#include "src/ui/utils/input_validation/passphrase_validator.h"

void continuously_ping(const std::array<unsigned char, 32> &pk_device, QObject* deviceRegister, const std::string& username) {
    const auto start_time = std::chrono::steady_clock::now();
    constexpr auto timeout_duration = std::chrono::minutes(2);

    while (true) {
        // Check if we've exceeded the timeout
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - start_time > timeout_duration) {
            QMetaObject::invokeMethod(deviceRegister, "pingTimeout", Qt::QueuedConnection);
            return;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
        try {
            qDebug() << "pinging for user: " << QString::fromStdString(username);
            if (post_check_user_exists(username, pk_device.data())) {
                std::cout << "logged in" << std::endl;

                // Emit signal to main thread using QMetaObject::invokeMethod
                QMetaObject::invokeMethod(deviceRegister, "userRegistered", Qt::QueuedConnection);
                return; // Thread terminates here
            }
        } catch (const std::exception &e) {
            qDebug() << "Ping failed:" << e.what();
        }
    }
}

DeviceRegister::DeviceRegister(const std::string &auth_code, const QImage &qr_code, QWidget *parent,
                               const unsigned char *pk_device, std::unique_ptr<SecureMemoryBuffer> sk_device,
                               const std::string &username)
    : QWidget(parent)
      , ui(new Ui::DeviceRegister)
      , m_auth_code(auth_code)
      , m_username(username)
      , m_sk_device(std::move(sk_device)) {
    // Start the pinging thread if pk_device is provided
    if (pk_device) {
        std::memcpy(m_pk_device.data(), pk_device, crypto_sign_PUBLICKEYBYTES);
        std::thread t1(continuously_ping, m_pk_device, this, username);
        t1.detach();
    }

    ui->setupUi(this);
    setupConnections();
    displayQRCode(qr_code);
    displayAuthCode(auth_code);
}

DeviceRegister::~DeviceRegister() {
    delete ui;
}

void DeviceRegister::setupConnections() {
    connect(ui->backButton, &QPushButton::clicked, this, &DeviceRegister::onBackButtonClicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &DeviceRegister::onCopyButtonClicked);
    connect(this, &DeviceRegister::userRegistered, this, &DeviceRegister::onUserRegistered);
    connect(this, &DeviceRegister::pingTimeout, this, &DeviceRegister::onPingTimeout);
}

void DeviceRegister::displayQRCode(const QImage &qr_code) const {
    if (!qr_code.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(qr_code);
        ui->qrCodeLabel->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        std::cout << "QR code is null!" << std::endl;
    }
}

void DeviceRegister::displayAuthCode(const std::string &auth_code) const {
    int partLen = auth_code.length() / 4;
    QString code = QString::fromStdString(auth_code);
    ui->codeEdit1->setText(code.mid(0, partLen));
    ui->codeEdit2->setText(code.mid(partLen, partLen));
    ui->codeEdit3->setText(code.mid(2 * partLen, partLen));
    ui->codeEdit4->setText(code.mid(3 * partLen));
}

void DeviceRegister::onBackButtonClicked() {
    WindowManager::instance().showLogin();
}

void DeviceRegister::onCopyButtonClicked() {
    try {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString::fromStdString(m_auth_code));
        StyledMessageBox::success(this, "Copied to Clipboard",
                                  "All codes have been successfully copied to your clipboard");
    } catch (const std::exception &e) {
        StyledMessageBox::error(this, "Copy Failed",
                                "Failed to copy codes to clipboard" + QString::fromStdString(e.what()));
    }
}

void DeviceRegister::onUserRegistered() {
    QString errorMessage;
    QString passphrase;
    QString confirmPassphrase;

    // Keep showing the dialog until a valid passphrase is entered or user cancels
    do {
        confirmPassphrase = StyledMessageBox::getPassphraseWithVerification(this, errorMessage);

        if (confirmPassphrase.isEmpty()) {
            StyledMessageBox::error(this, "Error", "Passphrase confirmation is required");
            return;
        }

        if (PassphraseValidator::validate(passphrase, confirmPassphrase, errorMessage)) {
            break;
        }

        StyledMessageBox::error(this, "Invalid Passphrase", errorMessage);
    } while (true);

    try {
        // Convert QString to SecureMemoryBuffer
        auto securePassphrase = SecureMemoryBuffer::create(passphrase.length());
        memcpy(securePassphrase->data(), passphrase.toUtf8().constData(), passphrase.length());

        set_up_client_for_user(m_username, std::move(securePassphrase));

        randombytes_buf(m_nonce, CHA_CHA_NONCE_LEN);
        const auto esk_device = encrypt_secret_key(m_sk_device, m_nonce);
        save_encrypted_keypair("device", m_pk_device.data(), esk_device, m_nonce);

        // Create a new SecureMemoryBuffer for login
        auto loginPassphrase = SecureMemoryBuffer::create(passphrase.length());
        memcpy(loginPassphrase->data(), passphrase.toUtf8().constData(), passphrase.length());

        // Clear the QString after copying
        passphrase.fill('\0');

        login_user(m_username, std::move(loginPassphrase));

        WindowManager::instance().showReceived();
    } catch (const std::exception &e) {
        qDebug() << "Login failed:" << e.what();
    }
}

void DeviceRegister::onPingTimeout()
{
    StyledMessageBox::error(this, "Timeout", "Took too long, returning to login");
    WindowManager::instance().showLogin();
}
