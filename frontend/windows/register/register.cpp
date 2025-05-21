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
    // for preventing buffer overflow attacks
    const int MAX_INPUT_LENGTH = 255;
    
    QString fullName = ui->fullNameEdit->text().left(MAX_INPUT_LENGTH);
    QString username = ui->usernameEdit->text().left(MAX_INPUT_LENGTH);
    QString password = ui->passwordEdit->text().left(MAX_INPUT_LENGTH);
    QString confirmPassword = ui->confirmPasswordEdit->text().left(MAX_INPUT_LENGTH);
    
    ui->fullNameEdit->setText(fullName);
    ui->usernameEdit->setText(username);
    ui->passwordEdit->setText(password);
    ui->confirmPasswordEdit->setText(confirmPassword);
    
    // Sanitize inputs - manually remove potentially dangerous characters
    for (QChar c : QString(";'\"")) {
        fullName.remove(c);
        username.remove(c);
    }
    
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


    if (password.length() < 20) {
        StyledMessageBox::warning(this, "Error", "Password must be at least 20 characters long");
        return;
    }

    bool hasUppercase = false;
    for (QChar c : password) {
        if (c.isUpper()) {
            hasUppercase = true;
            break;
        }
    }

    if (!hasUppercase) {
        StyledMessageBox::warning(this, "Error", "Password must contain at least one uppercase letter");
        return;
    }

    bool hasLowercase = false;
    for (QChar c : password) {
        if (c.isLower()) {
            hasLowercase = true;
            break;
        }
    }
    if (!hasLowercase) {
        StyledMessageBox::warning(this, "Error", "Password must contain at least one lowercase letter");
        return;
    }

    bool hasSpecial = false;
    for (QChar c : password) {
        if (!c.isLetterOrNumber()) {
            hasSpecial = true;
            break;
        }
    }
    if (!hasSpecial) {
        StyledMessageBox::warning(this, "Error", "Password must contain at least one special character");
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
    // Close the register window
    this->close();

    
} 