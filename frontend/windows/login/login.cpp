#include "./login.h"
#include "ui_login.h"
#include "../register/register.h"
#include "../../utils/messagebox.h"


Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    setupConnections();
}

Login::~Login()
{
    delete ui;
}

void Login::setupConnections()
{
    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &Login::onRegisterButtonClicked);
}

void Login::onLoginButtonClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        StyledMessageBox::warning(this, "Error", "Please enter both username and password");
        return;
    }

    if (username.length() < 3) {
        StyledMessageBox::warning(this, "Error", "Username must be at least 3 characters long");
        return;
    }

    if (password.length() < 20) {
        StyledMessageBox::warning(this, "Error", "Password must be at least 20 characters long");
        return;
    }

    StyledMessageBox::info(this, "Success", "Login functionality here");
}

void Login::onRegisterButtonClicked()
{
    Register* registerWindow = new Register();
    // Pass the login window reference to the register window
    registerWindow->setLoginWindow(this);
    registerWindow->show();
    this->hide();
    // Make sure the register window gets deleted when closed
    registerWindow->setAttribute(Qt::WA_DeleteOnClose);
} 