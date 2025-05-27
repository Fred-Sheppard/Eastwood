#include "send_file.h"
#include "ui_send_file.h"
#include "../../utils/messagebox.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QTimer>

SendFile::SendFile(QWidget *parent, QWidget* receivedWindow)
    : QWidget(parent)
    , ui(new Ui::SendFile)
    , m_receivedWindow(receivedWindow)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
}

SendFile::~SendFile()
{
    delete ui;
}

void SendFile::setupUI()
{
    // The nav bar styling is already handled in the UI file
    // We only need to style the form container
    ui->formContainer->setStyleSheet(R"(
        QWidget {
            background-color: white;
        }
        QLineEdit {
            padding: 8px 12px;
            font-size: 14px;
            border-radius: 6px;
            background-color: white;
            border: 1px solid #dfe6e9;
            color: #2d3436;
            margin: 4px 0;
            min-height: 20px;
        }
        QLineEdit:focus {
            border: 2px solid #6c5ce7;
        }
        QPushButton {
            font-size: 14px;
            padding: 8px 16px;
            border-radius: 6px;
            margin: 4px 0;
        }
        QPushButton#sendButton {
            font-weight: bold;
            color: white;
            background-color: #6c5ce7;
            border: none;
            min-height: 36px;
        }
        QPushButton#sendButton:hover {
            background-color: #5049c9;
        }
        QPushButton#sendButton:pressed {
            background-color: #4040b0;
        }
        QPushButton#browseButton {
            color: #2d3436;
            background-color: white;
            border: 1px solid #dfe6e9;
            min-height: 32px;
            max-height: 32px;
            font-size: 13px;
            padding: 4px 12px;
        }
        QPushButton#browseButton:hover {
            background-color: #f1f2f6;
            border-color: #6c5ce7;
            color: #6c5ce7;
        }
        QLabel {
            color: #2d3436;
            font-weight: bold;
            font-size: 14px;
            margin: 8px 0 4px 0;
        }
        QLabel#fileDetailsLabel {
            font-size: 13px;
            font-weight: normal;
            color: #636e72;
            padding: 12px;
            background-color: #f8f9fa;
            border: 1px solid #dfe6e9;
            border-radius: 6px;
            margin: 8px 0;
            line-height: 1.4;
        }
    )");

    ui->fileDetailsLabel->hide();
    ui->sendButton->setEnabled(false);
}

void SendFile::setupConnections()
{
    connect(ui->browseButton, &QPushButton::clicked, this, &SendFile::onBrowseClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &SendFile::onSendClicked);
    connect(ui->usernameInput, &QLineEdit::textChanged, this, &SendFile::updateSendButtonState);
    connect(ui->filePathInput, &QLineEdit::textChanged, this, &SendFile::updateSendButtonState);
    
    // Navigation connections
    connect(ui->receivedButton, &QPushButton::clicked, this, &SendFile::onReceivedButtonClicked);
    connect(ui->sentButton, &QPushButton::clicked, this, &SendFile::onSentButtonClicked);
    connect(ui->sendFileButton, &QPushButton::clicked, this, &SendFile::onSendFileButtonClicked);
    connect(ui->settingsButton, &QPushButton::clicked, this, &SendFile::onSettingsButtonClicked);
}

void SendFile::onBrowseClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File");
    if (!filePath.isEmpty()) {
        ui->filePathInput->setText(filePath);
        updateFileDetails(filePath);
    }
}

void SendFile::updateFileDetails(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString details = QString(
        "File Details:\n\n"
        "Name: %1\n"
        "Size: %2 KB\n"
        "Type: %3\n"
        "Created: %4\n"
        "Last Modified: %5\n"
        "Path: %6"
    ).arg(fileInfo.fileName())
     .arg(QString::number(fileInfo.size() / 1024.0, 'f', 2))
     .arg(fileInfo.suffix().toUpper())
     .arg(fileInfo.birthTime().toString("yyyy-MM-dd hh:mm:ss"))
     .arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"))
     .arg(fileInfo.absolutePath());
    
    ui->fileDetailsLabel->setText(details);
    ui->fileDetailsLabel->show();
}

void SendFile::onSendClicked()
{
    QString username = ui->usernameInput->text();
    QString filePath = ui->filePathInput->text();
    
    // TODO: Implement actual file sending logic
    StyledMessageBox::info(this, "File Sending",
                       QString("Sending file to %1:\n%2")
                       .arg(username)
                       .arg(filePath));
    
    // Switch back to received view after sending
    onReceivedButtonClicked();
}

void SendFile::updateSendButtonState()
{
    ui->sendButton->setEnabled(!ui->usernameInput->text().isEmpty() && 
                             !ui->filePathInput->text().isEmpty());
}

void SendFile::onReceivedButtonClicked()
{
    if (m_receivedWindow) {
        m_receivedWindow->show();
        deleteLater(); // This will delete the SendFile window after the event loop processes it
    }
}

void SendFile::onSentButtonClicked()
{
    // TODO: Create and show sent files window
    // For now, just show a message
    StyledMessageBox::info(this, "Coming Soon", "Sent Files view will be implemented soon.");
}

void SendFile::onSendFileButtonClicked()
{
    // Already on send file view, do nothing
}

void SendFile::onSettingsButtonClicked()
{
    // TODO: Create and show settings window
    // For now, just show a message
    StyledMessageBox::info(this, "Coming Soon", "Settings view will be implemented soon.");
}

void SendFile::onSendFileNavClicked()
{
    // TODO: Switch to send file page
}
