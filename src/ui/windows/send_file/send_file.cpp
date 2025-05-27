#include "send_file.h"
#include "ui_send_file.h"
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

SendFile::SendFile(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SendFile)
{
    ui->setupUi(this);
    setupConnections();

    // Connect WindowManager signal to handle navbar highlighting
    connect(&WindowManager::instance(), &WindowManager::windowShown,
            this, &SendFile::onWindowShown);
}

SendFile::~SendFile()
{
    delete ui;
}

void SendFile::setupConnections()
{
    connect(ui->browseButton, &QPushButton::clicked, this, &SendFile::onBrowseClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &SendFile::onSendClicked);

    // Connect NavBar signals
    NavBar* navbar = findChild<NavBar*>();
    if (navbar) {
        connect(navbar, &NavBar::receivedClicked, this, &SendFile::onReceivedButtonClicked);
        connect(navbar, &NavBar::sentClicked, this, &SendFile::onSentButtonClicked);
        connect(navbar, &NavBar::sendFileClicked, this, &SendFile::onSendFileButtonClicked);
        connect(navbar, &NavBar::settingsClicked, this, &SendFile::onSettingsButtonClicked);
    }
}

void SendFile::onBrowseClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File", "", "All Files (*.*)");
    if (!filePath.isEmpty()) {
        ui->filePathInput->setText(filePath);
    }
}

void SendFile::onSendClicked()
{
    QString filePath = ui->filePathInput->text();
    if (filePath.isEmpty()) {
        StyledMessageBox::warning(this, "No File Selected", "Please select a file to send.");
        return;
    }

    // TODO: Implement file sending logic
    StyledMessageBox::info(this, "File Sent", "File has been sent successfully!");
}

void SendFile::navigateTo(QWidget* newWindow)
{
    newWindow->setParent(this->parentWidget());  // Set the same parent
    newWindow->show();
    this->setAttribute(Qt::WA_DeleteOnClose);  // Mark for deletion when closed
    close();  // This will trigger deletion due to WA_DeleteOnClose
}

void SendFile::onReceivedButtonClicked()
{
    WindowManager::instance().showReceived();
    hide();
}

void SendFile::onSentButtonClicked()
{
    WindowManager::instance().showSent();
    hide();
}

void SendFile::onSendFileButtonClicked()
{
    WindowManager::instance().showSendFile();
    hide();
}

void SendFile::onSettingsButtonClicked()
{
    WindowManager::instance().showSettings();
    hide();
}

void SendFile::onWindowShown(const QString& windowName)
{
    // Find the navbar and update its active button
    NavBar* navbar = findChild<NavBar*>();
    if (navbar) {
        navbar->setActiveButton(windowName);
    }
}
