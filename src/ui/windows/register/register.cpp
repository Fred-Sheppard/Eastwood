#include "./register.h"
#include "ui_register.h"
#include "src/ui/utils/messagebox.h"
#include "src/ui/utils/window_manager/window_manager.h"
#include "src/auth/register_user/register_user.h"
#include <iostream>
#include "src/auth/register_device/register_device.h"
#include "src/endpoints/endpoints.h"
#include "src/utils/JsonParser.h"
#include <QScreen>
#include <QApplication>
#include <sodium.h>
#include <QByteArray>
#include "src/key_exchange/utils.h"
#include "src/ui/utils/qr_code_generation/QRCodeGenerator.h"
#include "src/keys/secure_memory_buffer.h"


Register::Register(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Register)
{
    ui->setupUi(this);
    setupConnections();
    
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

Register::~Register()
{
    delete ui;
}

void Register::setupConnections()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &Register::onLoginButtonClicked);
    connect(ui->togglePassphraseButton, &QPushButton::clicked, this, &Register::onTogglePassphraseClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &Register::onRegisterButtonClicked);
    connect(ui->deviceRegisterButton, &QPushButton::clicked, this, &Register::onDeviceRegisterButtonClicked);
}

void Register::onRegisterButtonClicked()
{
    // passphrase requirements as per NIST SP 800-63B guidelines
    const int MAX_PASSPHRASE_LENGTH = 64;
    const int MIN_PASSPHRASE_LENGTH = 20;
    const int MAX_INPUT_LENGTH = 64;

    QString fullName = ui->fullNameEdit->text().left(MAX_INPUT_LENGTH);
    QString username = ui->usernameEdit->text().left(MAX_INPUT_LENGTH);
    QString passphrase = ui->passphraseEdit->text().left(MAX_PASSPHRASE_LENGTH);
    QString confirmPassphrase = ui->confirmPassphraseEdit->text().left(MAX_PASSPHRASE_LENGTH);

    ui->fullNameEdit->setText(fullName);
    ui->usernameEdit->setText(username);
    ui->passphraseEdit->setText(passphrase);
    ui->confirmPassphraseEdit->setText(confirmPassphrase);

    if (fullName.isEmpty() || username.isEmpty() || passphrase.isEmpty() || confirmPassphrase.isEmpty()) {
        StyledMessageBox::warning(this, "Error", "Please fill in all fields");
        return;
    }

    if (username.length() < 3) {
        StyledMessageBox::warning(this, "Error", "Username must be at least 3 characters long");
        return;
    }

    if (fullName.length() > MAX_INPUT_LENGTH || username.length() > MAX_INPUT_LENGTH) {
        StyledMessageBox::warning(this, "Error", "Input too long");
        return;
    }

    if (passphrase.length() < MIN_PASSPHRASE_LENGTH) {
        StyledMessageBox::warning(this, "Error", "Passphrase must be at least 20 characters long");
        return;
    }

    if (passphrase.length() > MAX_PASSPHRASE_LENGTH) {
        StyledMessageBox::warning(this, "Error", "Passphrase cannot be longer than 64 characters");
        return;
    }

    // frontend passphrase logic
    if (passphrase != confirmPassphrase) {
        StyledMessageBox::warning(this, "Error", "Passphrases do not match");
        return;
    }

    try {
        register_user(username.toStdString(), std::make_unique<std::string>(passphrase.toStdString()));
        register_first_device();
        StyledMessageBox::success(this, "Success", "Hey " + username + "! You're registered and ready to go!");
        WindowManager::instance().showLogin();
        
        
    } catch (const webwood::HttpError& e) {
        const std::string errorBody = e.what();
        const bool isHtmlError = errorBody.find("<!DOCTYPE HTML") != std::string::npos;
        
        const QString title = isHtmlError ? "Server Unavailable" : "Registration Failed";
        const QString message = isHtmlError 
            ? "The server is currently unavailable. Please try again later."
            : QString("Registration failed: %1").arg(QString::fromStdString(errorBody));
        
        StyledMessageBox::error(this, title, message);
    } catch (const std::exception& e) {
        StyledMessageBox::error(this, "Registration Failed", 
            QString("An error occurred: %1").arg(e.what()));
    }
}

void Register::onLoginButtonClicked()
{
    WindowManager::instance().showLogin();
    
    
}

void Register::onTogglePassphraseClicked()
{
    m_passphraseVisible = !m_passphraseVisible;
    ui->passphraseEdit->setEchoMode(m_passphraseVisible ? QLineEdit::Normal : QLineEdit::Password);
    ui->confirmPassphraseEdit->setEchoMode(m_passphraseVisible ? QLineEdit::Normal : QLineEdit::Password);
    ui->togglePassphraseButton->setText(m_passphraseVisible ? "Hide" : "Show");
}

void Register::onDeviceRegisterButtonClicked()
{
    if (sodium_init() < 0) {
        throw std::runtime_error("Libsodium initialization failed");
    }

    unsigned char pk_device[crypto_sign_PUBLICKEYBYTES];
    auto sk_device = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);

    crypto_sign_keypair(pk_device, sk_device->data());
    std::string auth_code = bin2base64(pk_device, crypto_sign_PUBLICKEYBYTES);
    QImage qr_code = getQRCodeForMyDevicePublicKey(auth_code);
    
    if (auth_code.empty()) {
        StyledMessageBox::error(this, "Device Registration Failed", "Failed to generate authentication code");
        return;
    }
    
    if (qr_code.isNull()) {
        StyledMessageBox::error(this, "Device Registration Failed", "Failed to generate QR code");
        return;
    }
    
    WindowManager::instance().showDeviceRegister(auth_code, qr_code);
} 
