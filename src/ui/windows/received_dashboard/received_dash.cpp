#include "received_dash.h"
#include "ui_received_dash.h"
#include "src/ui/utils/messagebox.h"
#include "src/ui/utils/window_manager/window_manager.h"
#include "src/ui/utils/navbar/navbar.h"
#include "src/ui/windows/sent_dashboard/sent_dash.h"
#include <QTimer>
#include <QCheckBox>
#include <iostream>

#include "src/communication/ReceiveFlow.h"

Received::Received(QWidget *parent, QWidget* sendFileWindow)
    : QWidget(parent)
    , ui(new Ui::Received)
    , m_sendFileWindow(sendFileWindow)
{
    ui->setupUi(this);
    setupConnections();
    setupFileList();
    refreshFileList();
}

Received::~Received()
{
    delete ui;
}

void Received::setupConnections()
{
    connect(ui->navBar, &NavBar::receivedClicked, this, &Received::onReceivedButtonClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &Received::onSendButtonClicked);
}

void Received::setupFileList() const {
    ui->fileList->setSpacing(2);
    ui->fileList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->fileList->setSelectionMode(QAbstractItemView::NoSelection);
}

void Received::addFileItem(const QString& fileName,
                         const QString& fileSize,
                         const QString& timestamp,
                         const QString& owner)
{
    auto* item = new QListWidgetItem(ui->fileList);
    auto* widget = new FileItemWidget(fileName, fileSize, timestamp, owner, 
                                    FileItemWidget::Mode::Received, this);

    connect(widget, &FileItemWidget::fileClicked, this, &Received::onFileItemClicked);
    connect(widget, &FileItemWidget::downloadFileClicked, this, &Received::onDownloadFileClicked);

    item->setSizeHint(widget->sizeHint());
    ui->fileList->addItem(item);
    ui->fileList->setItemWidget(item, widget);
}

void Received::refreshFileList()
{
    ui->fileList->clear();

    // update_handshakes();
    // update_messages();

    // auto metadata = get_file_metadata();
    //
    // for (const auto& [file_name, file_size, mime_type] : metadata) {
    //     std::string file_size_str = std::to_string(file_size);
    //     addFileItem(QString::fromStdString(file_name), QString::fromStdString(file_size_str), "sadfa", "asdfadf");
    // }
}

void Received::onSendButtonClicked()
{
    WindowManager::instance().showSendFile();
}

void Received::onFileItemClicked(const FileItemWidget* widget)
{
    showFileMetadata(widget);
}

void Received::showFileMetadata(const FileItemWidget* widget)
{
    StyledMessageBox::info(this, "File Details",
                       QString("File Details:\n\nName: %1\nSize: %2\nShared by: %3\nTimestamp: %4")
                       .arg(widget->getFileName())
                       .arg(widget->getFileSize())
                       .arg(widget->getOwner())
                       .arg(widget->getTimestamp()));
}

void Received::onDownloadFileClicked(FileItemWidget* widget)
{
    StyledMessageBox::info(this, "Not Implemented", "Download functionality is not yet implemented.");
}

void Received::onReceivedButtonClicked()
{
    std::cout << "Received button clicked" << std::endl;
    refreshFileList();
}

