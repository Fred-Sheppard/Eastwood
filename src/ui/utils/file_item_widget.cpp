#include "file_item_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>

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
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 16, 20, 16);

    // File icon with file type indicator
    auto* fileIconContainer = new QWidget(this);
    fileIconContainer->setFixedSize(42, 42);
    fileIconContainer->setStyleSheet(R"(
        background-color: #f0eeff;
        border-radius: 8px;
    )");
    
    auto* fileTypeLabel = new QLabel(getFileTypeAbbreviation(fileName), fileIconContainer);
    fileTypeLabel->setAlignment(Qt::AlignCenter);
    fileTypeLabel->setStyleSheet(R"(
        font-size: 12px;
        font-weight: bold;
        color: #6c5ce7;
    )");
    fileTypeLabel->setFixedSize(42, 42);
    
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
    revokeButton->setFixedSize(75, 30);
    revokeButton->setCursor(Qt::PointingHandCursor);
    revokeButton->setStyleSheet(R"(
        QPushButton {
            font-size: 12px;
            color: #e74c3c;
            background-color: #fff1f0;
            border: none;
            border-radius: 6px;
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
    deleteButton->setFixedSize(75, 30);
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setStyleSheet(R"(
        QPushButton {
            font-size: 12px;
            color: #e74c3c;
            background-color: #fff1f0;
            border: none;
            border-radius: 6px;
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

    mainLayout->addWidget(fileIconContainer);
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