#include "settings.h"
#include "ui_settings.h"
#include "../../utils/messagebox.h"
#include "../../utils/window_manager/window_manager.h"
#include "../../utils/navbar/navbar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QLineEdit>
#include <QDialog>
#include <QScrollArea>
#include <QTimer>
#include <QCheckBox>

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    setupConnections();

    // Connect WindowManager signal to handle navbar highlighting
    connect(&WindowManager::instance(), &WindowManager::windowShown,
            this, &Settings::onWindowShown);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setupConnections()
{
    // Connect password fields to validation
    connect(ui->currentPassword, &QLineEdit::textChanged, this, &Settings::validatePassword);
    connect(ui->newPassword, &QLineEdit::textChanged, this, &Settings::validatePassword);
    connect(ui->confirmPassword, &QLineEdit::textChanged, this, &Settings::validatePassword);

    // Connect buttons
    connect(ui->cancelButton, &QPushButton::clicked, this, &Settings::onCancelClicked);

    // Connect NavBar signals
    NavBar* navbar = findChild<NavBar*>();
    if (navbar) {
        connect(navbar, &NavBar::receivedClicked, this, &Settings::onReceivedButtonClicked);
        connect(navbar, &NavBar::sentClicked, this, &Settings::onSentButtonClicked);
        connect(navbar, &NavBar::sendFileClicked, this, &Settings::onSendFileButtonClicked);
        connect(navbar, &NavBar::logoutClicked, this, &Settings::onLogoutButtonClicked);
        connect(navbar, &NavBar::settingsClicked, this, &Settings::onSettingsButtonClicked);
    }
}

void Settings::validatePassword()
{
    QString newPassword = ui->newPassword->text();
    QString confirmPassword = ui->confirmPassword->text();

    if (newPassword.isEmpty() && confirmPassword.isEmpty()) {
        ui->passwordRequirements->setText("Password must be between 8 and 64 characters");
        ui->passwordRequirements->setStyleSheet("font-size: 12px; color: #636e72; margin-top: 5px;");
        return;
    }

    if (newPassword == confirmPassword) {
        ui->passwordRequirements->setText("Passwords match");
        ui->passwordRequirements->setStyleSheet("font-size: 12px; color: #27ae60; margin-top: 5px;");
    } else {
        ui->passwordRequirements->setText("Passwords do not match");
        ui->passwordRequirements->setStyleSheet("font-size: 12px; color: #e74c3c; margin-top: 5px;");
    }
}

void Settings::navigateTo(QWidget* newWindow)
{
    newWindow->setParent(this->parentWidget());  // Set the same parent
    newWindow->show();
    this->setAttribute(Qt::WA_DeleteOnClose);  // Mark for deletion when closed
    close();  // This will trigger deletion due to WA_DeleteOnClose
}

void Settings::onReceivedButtonClicked()
{
    ui->currentPassword->clear();
    ui->newPassword->clear();
    ui->confirmPassword->clear();
    WindowManager::instance().showReceived();
    hide();
}

void Settings::onSentButtonClicked()
{
    ui->currentPassword->clear();
    ui->newPassword->clear();
    ui->confirmPassword->clear();
    WindowManager::instance().showSent();
    hide();
}

void Settings::onSendFileButtonClicked()
{   
    ui->currentPassword->clear();
    ui->newPassword->clear();
    ui->confirmPassword->clear();
    WindowManager::instance().showSendFile();
    hide();
}

void Settings::onSettingsButtonClicked()
{   
    ui->currentPassword->clear();
    ui->newPassword->clear();
    ui->confirmPassword->clear();
}

void Settings::onWindowShown(const QString& windowName)
{
    // Find the navbar and update its active button
    NavBar* navbar = findChild<NavBar*>();
    if (navbar) {
        navbar->setActiveButton(windowName);
    }
}

void Settings::onCancelClicked()
{
    // Clear all password fields
    ui->currentPassword->clear();
    ui->newPassword->clear();
    ui->confirmPassword->clear();
    
    // Reset password requirements text
    ui->passwordRequirements->setText("Password must be between 8 and 64 characters");
    ui->passwordRequirements->setStyleSheet("font-size: 12px; color: #636e72; margin-top: 5px;");
    
    // Navigate back to the previous window
    WindowManager::instance().showReceived();
    hide();
}

void Settings::onLogoutButtonClicked() {
    // TODO: Implement logout functionality
    StyledMessageBox::info(this, "Not Implemented", "Logout functionality is not yet implemented.");
}
