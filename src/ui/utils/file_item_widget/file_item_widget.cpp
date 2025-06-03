#include "file_item_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QStyle>
#include <QIcon>

FileItemWidget::FileItemWidget(const QString& fileName,
                             const QString& fileSize,
                             const QString& timestamp,
                             const QString& owner,
                             const QString& uuid,
                             Mode mode,
                             QWidget* parent)
    : QWidget(parent)
    , fileName(fileName)
    , fileSize(fileSize)
    , uuid(uuid)
    , timestamp(timestamp)
    , owner(owner)
    , mode(mode)
    , fileNameLabel(nullptr)
    , detailsLabel(nullptr)
    , fileTypeLabel(nullptr)
    , fileIconContainer(nullptr)
    , revokeButton(nullptr)
    , deleteButton(nullptr)
    , downloadButton(nullptr)
    , mainLayout(nullptr)
    , infoLayout(nullptr)
    , buttonLayout(nullptr)
{
    setupUI();
    setupConnections();
}

void FileItemWidget::setupUI()
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 16, 20, 16);

    // File icon with file type indicator
    fileIconContainer = new QWidget(this);
    fileIconContainer->setFixedSize(42, 42);
    fileIconContainer->setStyleSheet(R"(
        background-color: #f5f6fa;
        border-radius: 8px;
    )");
    
    fileTypeLabel = new QLabel(getFileTypeAbbreviation(fileName), fileIconContainer);
    fileTypeLabel->setAlignment(Qt::AlignCenter);
    fileTypeLabel->setStyleSheet(R"(
        font-size: 12px;
        font-weight: bold;
        color: #6c5ce7;
    )");
    fileTypeLabel->setFixedSize(42, 42);
    
    // File info section
    infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(4);

    fileNameLabel = new QLabel(fileName.isNull() ? "Unknown File" : fileName, this);
    fileNameLabel->setStyleSheet("font-size: 15px; font-weight: 500; color: #2d3436;");

    // Safely construct the details string with null checks and simpler separator
    QString safeFileSize = fileSize.isNull() || fileSize.isEmpty() ? "Unknown size" : fileSize;
    QString safeTimestamp = timestamp.isNull() || timestamp.isEmpty() ? "Unknown time" : timestamp;
    QString safeOwner = owner.isNull() || owner.isEmpty() ? "Unknown owner" : owner;
    
    QString detailsText = QString("%1 | %2 | Shared by %3")
                         .arg(safeFileSize)
                         .arg(safeTimestamp)
                         .arg(safeOwner);

    detailsLabel = new QLabel(detailsText, this);
    detailsLabel->setStyleSheet("font-size: 13px; color: #636e72;");

    infoLayout->addWidget(fileNameLabel);
    infoLayout->addWidget(detailsLabel);

    // Action buttons
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    buttonLayout->addStretch();

    // Add download button - remove the problematic icon for now
    downloadButton = new QPushButton("Download", this);
    downloadButton->setFixedSize(80, 30);
    downloadButton->setCursor(Qt::PointingHandCursor);
    downloadButton->setStyleSheet(R"(
        QPushButton {
            background-color: #6c5ce7;
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #5049c9;
        }
        QPushButton:pressed {
            background-color: #4040b0;
        }
    )");
    buttonLayout->addWidget(downloadButton);

    // Only create and show revoke button in Sent mode
    if (mode == Mode::Sent) {
        revokeButton = new QPushButton("Revoke", this);
        revokeButton->setFixedSize(75, 30);
        revokeButton->setCursor(Qt::PointingHandCursor);
        revokeButton->setStyleSheet(R"(
            QPushButton {
                background-color: #6c5ce7;
                color: white;
                border: none;
                border-radius: 4px;
                font-size: 12px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #5049c9;
            }
            QPushButton:pressed {
                background-color: #4040b0;
            }
        )");
        buttonLayout->addWidget(revokeButton);
    }

    // Only create and show delete button in Sent mode
    if (mode == Mode::Sent) {
        deleteButton = new QPushButton("Delete", this);
        deleteButton->setFixedSize(75, 30);
        deleteButton->setCursor(Qt::PointingHandCursor);
        deleteButton->setStyleSheet(R"(
            QPushButton {
                background-color: #e74c3c;
                color: white;
                border: none;
                border-radius: 4px;
                font-size: 12px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #c0392b;
            }
            QPushButton:pressed {
                background-color: #a93226;
            }
        )");
        buttonLayout->addWidget(deleteButton);
    }

    buttonLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    mainLayout->addWidget(fileIconContainer);
    mainLayout->addLayout(infoLayout, 1);
    mainLayout->addLayout(buttonLayout);

    setCursor(Qt::PointingHandCursor);
}

void FileItemWidget::setupConnections()
{
    connect(downloadButton, &QPushButton::clicked, [this]() {
        emit downloadFileClicked(this);
    });

    if (mode == Mode::Sent) {
        connect(revokeButton, &QPushButton::clicked, [this]() {
            emit revokeAccessClicked(this);
        });
        connect(deleteButton, &QPushButton::clicked, [this]() {
            emit deleteFileClicked(this);
        });
    }
}

QString FileItemWidget::getFileTypeAbbreviation(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    QString ext = fileInfo.suffix().toLower();
    
    if (ext == "pdf") return "PDF";
    if (ext == "doc" || ext == "docx") return "DOC";
    if (ext == "xls" || ext == "xlsx") return "XLS";
    if (ext == "ppt" || ext == "pptx") return "PPT";
    if (ext == "txt") return "TXT";
    if (ext == "zip" || ext == "rar") return "ZIP";
    if (ext == "jpg" || ext == "jpeg" || ext == "png") return "IMG";
    
    return ext.toUpper().left(3);
}