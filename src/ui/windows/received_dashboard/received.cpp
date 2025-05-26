#include "received.h"
#include "ui_received.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

// FileItemWidget implementation
FileItemWidget::FileItemWidget(const QString& fileName,
                             const QString& fileSize,
                             const QString& timestamp,
                             const QString& owner,
                             QWidget* parent)
    : QWidget(parent)
    , fileName(fileName)
    , fileSize(fileSize)
    , timestamp(timestamp)
    , owner(owner)
{
    setupUI();
    setupConnections();
}

void FileItemWidget::setupUI()
{
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 15, 20, 15);

    // File icon (placeholder for now)
    auto* fileIconLabel = new QLabel(this);
    fileIconLabel->setFixedSize(36, 36);
    fileIconLabel->setStyleSheet(R"(
        background-color: #6c5ce7;
        border-radius: 6px;
    )");
    
    // File info section
    auto* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(4);

    fileNameLabel = new QLabel(fileName, this);
    fileNameLabel->setStyleSheet("font-size: 15px; font-weight: 500; color: #2d3436;");

    detailsLabel = new QLabel(QString("%1 • %2 • Shared by %3")
                            .arg(fileSize)
                            .arg(timestamp)
                            .arg(owner), this);
    detailsLabel->setStyleSheet("font-size: 13px; color: #636e72;");

    infoLayout->addWidget(fileNameLabel);
    infoLayout->addWidget(detailsLabel);

    // Action buttons
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);

    revokeButton = new QPushButton("Revoke", this);
    revokeButton->setFixedSize(70, 28);
    revokeButton->setCursor(Qt::PointingHandCursor);
    revokeButton->setStyleSheet(R"(
        QPushButton {
            font-size: 12px;
            color: #e74c3c;
            background-color: #fff1f0;
            border: none;
            border-radius: 5px;
            padding: 0 8px;
        }
        QPushButton:hover {
            background-color: #ffe4e3;
        }
        QPushButton:pressed {
            background-color: #ffd7d5;
        }
    )");

    deleteButton = new QPushButton("Delete", this);
    deleteButton->setFixedSize(70, 28);
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setStyleSheet(R"(
        QPushButton {
            font-size: 12px;
            color: #e74c3c;
            background-color: #fff1f0;
            border: none;
            border-radius: 5px;
            padding: 0 8px;
        }
        QPushButton:hover {
            background-color: #ffe4e3;
        }
        QPushButton:pressed {
            background-color: #ffd7d5;
        }
    )");

    buttonLayout->addWidget(revokeButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    mainLayout->addWidget(fileIconLabel);
    mainLayout->addLayout(infoLayout, 1);
    mainLayout->addLayout(buttonLayout);

    setCursor(Qt::PointingHandCursor);
}

void FileItemWidget::setupConnections()
{
    connect(revokeButton, &QPushButton::clicked, [this]() {
        emit revokeAccessClicked(this);
    });

    connect(deleteButton, &QPushButton::clicked, [this]() {
        emit deleteFileClicked(this);
    });
}

// Received implementation
Received::Received(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Received)
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
    connect(ui->sendButton, &QPushButton::clicked, this, &Received::onSendButtonClicked);
    connect(ui->sentDashboardButton, &QPushButton::clicked, this, &Received::onSentDashboardClicked);
}

void Received::setupFileList()
{
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
    auto* widget = new FileItemWidget(fileName, fileSize, timestamp, owner, this);

    connect(widget, &FileItemWidget::revokeAccessClicked, this, &Received::onRevokeAccessClicked);
    connect(widget, &FileItemWidget::deleteFileClicked, this, &Received::onDeleteFileClicked);
    connect(widget, &FileItemWidget::fileClicked, this, &Received::onFileItemClicked);

    item->setSizeHint(widget->sizeHint());
    ui->fileList->addItem(item);
    ui->fileList->setItemWidget(item, widget);
}

void Received::refreshFileList()
{
    ui->fileList->clear();

    // TODO: Fetch actual files from server
    // Example data for demonstration
    addFileItem("Important Document.pdf", "2.5 MB", "2024-03-15 14:30", "John Doe");
    addFileItem("Project Presentation.pptx", "5.8 MB", "2024-03-14 09:15", "Alice Smith");
    addFileItem("Budget Report.xlsx", "1.2 MB", "2024-03-13 16:45", "Bob Johnson");
}

void Received::onSendButtonClicked()
{
    // TODO: Implement file sending dialog
}

void Received::onFileItemClicked(FileItemWidget* widget)
{
    showFileMetadata(widget);
}

void Received::onRevokeAccessClicked(FileItemWidget* widget)
{
    // TODO: Implement revoke access
    QMessageBox::information(this, "Revoke Access",
                           QString("Revoking access to file: %1").arg(widget->fileName));
}

void Received::onDeleteFileClicked(FileItemWidget* widget)
{
    // TODO: Implement file deletion
    QMessageBox::information(this, "Delete File",
                           QString("Deleting file: %1").arg(widget->fileName));
}

void Received::onSentDashboardClicked()
{
    // TODO: Switch to sent dashboard
}

void Received::showFileMetadata(FileItemWidget* widget)
{
    // TODO: Show detailed metadata dialog
    QMessageBox::information(this, "File Details",
                           QString("File: %1\nSize: %2\nShared by: %3\nTimestamp: %4")
                           .arg(widget->fileName)
                           .arg(widget->fileSize)
                           .arg(widget->owner)
                           .arg(widget->timestamp));
}

void Received::sendFileToUser(const QString& username, const QString& fileId)
{
    // TODO: Implement file sharing logic
}