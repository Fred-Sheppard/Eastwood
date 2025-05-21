#include "./register.h"
#include "ui_register.h"
#include "../../utils/messagebox.h"


Register::Register(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Register)
{
    ui->setupUi(this);
    setupConnections();
}

Register::~Register()
{
    delete ui;
}

void Register::setupConnections()
{
    connect(ui->registerButton, &QPushButton::clicked, this, &Register::onRegisterButtonClicked);
    connect(ui->loginButton, &QPushButton::clicked, this, &Register::onLoginButtonClicked);
}

void Register::onRegisterButtonClicked()
{
    // Password requirements as per NIST SP 800-63B guidelines
    const int MAX_PASSWORD_LENGTH = 64;
    const int MIN_PASSWORD_LENGTH = 8; 
    const int MAX_INPUT_LENGTH = 64;

    QString fullName = ui->fullNameEdit->text().left(MAX_INPUT_LENGTH);
    QString username = ui->usernameEdit->text().left(MAX_INPUT_LENGTH);
    QString password = ui->passwordEdit->text().left(MAX_PASSWORD_LENGTH);
    QString confirmPassword = ui->confirmPasswordEdit->text().left(MAX_PASSWORD_LENGTH);
    
    ui->fullNameEdit->setText(fullName);
    ui->usernameEdit->setText(username);
    ui->passwordEdit->setText(password);
    ui->confirmPasswordEdit->setText(confirmPassword);
    
    if (fullName.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
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

    if (password.length() < MIN_PASSWORD_LENGTH) {
        StyledMessageBox::warning(this, "Error", "Password must be at least 8 characters long");
        return;
    }

    if (password.length() > MAX_PASSWORD_LENGTH) {
        StyledMessageBox::warning(this, "Error", "Password cannot be longer than 64 characters");
        return;
    }

    // frontend password logic
    if (password != confirmPassword) {
        StyledMessageBox::warning(this, "Error", "Passwords do not match");
        return;
    }

    StyledMessageBox::info(this, "Success", "Registration functionality here");
}

void Register::onLoginButtonClicked()
{
    if (m_loginWindow) {
        m_loginWindow->show();
    }

    // TODO: do on backend for NIST SP 800-63B standard
    // 1. Blocklist commonly used passwords
    // 2. Passwords from known breaches
    // 3. Context-specific words (username, app name, etc.)
    // Do NOT implement complexity requirements (uppercase, numbers, special chars)

    this->close();
} 