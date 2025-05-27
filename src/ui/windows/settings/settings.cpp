#include "settings.h"
#include "ui_settings.h"
#include "../../utils/messagebox.h"
#include "../../utils/window_manager.h"

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    setupConnections();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setupConnections()
{
    connect(ui->saveButton, &QPushButton::clicked, this, &Settings::onSaveButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &Settings::onCancelButtonClicked);
    connect(ui->newPassword, &QLineEdit::textChanged, this, &Settings::validatePassword);
    connect(ui->confirmPassword, &QLineEdit::textChanged, this, &Settings::validatePassword);
    
    // Add navigation connections
    connect(ui->receivedButton, &QPushButton::clicked, this, &Settings::onReceivedButtonClicked);
    connect(ui->sentButton, &QPushButton::clicked, this, &Settings::onSentButtonClicked);
    connect(ui->sendFileButton, &QPushButton::clicked, this, &Settings::onSendFileButtonClicked);
}

void Settings::onSaveButtonClicked()
{
    QString currentPassword = ui->currentPassword->text();
    QString newPassword = ui->newPassword->text();
    QString confirmPassword = ui->confirmPassword->text();

    // TODO: Verify current password against stored password
    if (currentPassword.isEmpty()) {
        StyledMessageBox::error(this, "Error", "Please enter your current password");
        return;
    }

    if (!validatePasswordRequirements(newPassword)) {
        StyledMessageBox::error(this, "Error", "New password must be between 8 and 64 characters");
        return;
    }

    if (newPassword != confirmPassword) {
        StyledMessageBox::error(this, "Error", "New passwords do not match");
        return;
    }

    // TODO: Update password in secure storage
    StyledMessageBox::info(this, "Success", "Password updated successfully");
    close();
}

void Settings::onCancelButtonClicked()
{
    WindowManager::instance().showReceived();
    hide();
}

void Settings::validatePassword()
{
    QString newPassword = ui->newPassword->text();
    QString confirmPassword = ui->confirmPassword->text();

    bool isValid = validatePasswordRequirements(newPassword);
    bool passwordsMatch = newPassword == confirmPassword;

    ui->saveButton->setEnabled(isValid && passwordsMatch);
}

bool Settings::validatePasswordRequirements(const QString& password)
{
    return password.length() >= 8 && password.length() <= 64;
}

void Settings::onReceivedButtonClicked()
{
    WindowManager::instance().showReceived();
    hide();
}

void Settings::onSentButtonClicked()
{
    WindowManager::instance().showSent();
    hide();
}

void Settings::onSendFileButtonClicked()
{
    WindowManager::instance().showSendFile();
    hide();
}
