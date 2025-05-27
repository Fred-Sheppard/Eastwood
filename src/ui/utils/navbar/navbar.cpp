#include "navbar.h"
#include "ui_navbar.h"
#include "../../utils/window_manager/window_manager.h"

NavBar::NavBar(QWidget *parent) : QWidget(parent), ui(new Ui::NavBar) {
    ui->setupUi(this);
    setupConnections();
    // Set initial active button
    setActiveButton("receivedButton");
}

NavBar::~NavBar() { delete ui; }

void NavBar::setupConnections() {
    connect(ui->receivedButton, &QPushButton::clicked, this, &NavBar::receivedClicked);
    connect(ui->sentButton, &QPushButton::clicked, this, &NavBar::sentClicked);
    connect(ui->sendFileButton, &QPushButton::clicked, this, &NavBar::sendFileClicked);
    connect(ui->settingsButton, &QPushButton::clicked, this, &NavBar::settingsClicked);
}

void NavBar::setActiveButton(const QString& buttonName) {
    // Reset all buttons to inactive state
    updateButtonStyle(ui->receivedButton, false);
    updateButtonStyle(ui->sentButton, false);
    updateButtonStyle(ui->sendFileButton, false);
    updateButtonStyle(ui->settingsButton, false);

    // Set the active button
    if (buttonName == "receivedButton") {
        updateButtonStyle(ui->receivedButton, true);
    } else if (buttonName == "sentButton") {
        updateButtonStyle(ui->sentButton, true);
    } else if (buttonName == "sendFileButton") {
        updateButtonStyle(ui->sendFileButton, true);
    } else if (buttonName == "settingsButton") {
        updateButtonStyle(ui->settingsButton, true);
    }
}

void NavBar::updateButtonStyle(QPushButton* button, bool isActive) {
    if (isActive) {
        button->setStyleSheet(R"(
            QPushButton {
                font-size: 14px;
                font-weight: 500;
                color: white;
                background-color: #6c5ce7;
                border-radius: 8px;
                text-align: left;
                padding-left: 15px;
            }
            QPushButton:hover {
                background-color: #5049c9;
            }
            QPushButton:pressed {
                background-color: #4040b0;
            }
        )");
    } else {
        button->setStyleSheet(R"(
            QPushButton {
                font-size: 14px;
                color: #2d3436;
                background-color: transparent;
                border-radius: 8px;
                text-align: left;
                padding-left: 15px;
            }
            QPushButton:hover {
                background-color: #f1f2f6;
            }
            QPushButton:pressed {
                background-color: #dfe6e9;
            }
        )");
    }
}

void NavBar::onReceivedButtonClicked() {
    WindowManager::instance().showReceived();
    setActiveButton("receivedButton");
    hide();
}

void NavBar::onSentButtonClicked() {
    WindowManager::instance().showSent();
    setActiveButton("sentButton");
    hide();
}

void NavBar::onSendFileButtonClicked() {
    WindowManager::instance().showSendFile();
    setActiveButton("sendFileButton");
    hide();
}

void NavBar::onSettingsButtonClicked() {
    WindowManager::instance().showSettings();
    setActiveButton("settingsButton");
    hide();
}
