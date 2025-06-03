#ifndef FILE_ITEM_WIDGET_H
#define FILE_ITEM_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QHBoxLayout>
#include <QVBoxLayout>

class FileItemWidget : public QWidget {
    Q_OBJECT

public:
    enum class Mode {
        Received,
        Sent
    };

    explicit FileItemWidget(const QString& fileName, 
                          const QString& fileSize, 
                          const QString& timestamp,
                          const QString& owner,
                          const QString& uuid,
                          Mode mode = Mode::Received,
                          QWidget* parent = nullptr);

    // Add getter methods
    QString getFileName() const { return fileName; }
    QString getFileSize() const { return fileSize; }
    QString getTimestamp() const { return timestamp; }
    QString getOwner() const { return owner; }
    QString getUuid() const { return uuid; }

    // Override sizeHint to provide a safe default size
    QSize sizeHint() const override { return QSize(400, 80); }

signals:
    void revokeAccessClicked(FileItemWidget* widget);
    void deleteFileClicked(FileItemWidget* widget);
    void fileClicked(FileItemWidget* widget);
    void downloadFileClicked(FileItemWidget* widget);

private:
    QLabel* fileNameLabel;
    QLabel* detailsLabel;
    QLabel* fileTypeLabel;
    QWidget* fileIconContainer;
    QPushButton* revokeButton;
    QPushButton* deleteButton;
    QPushButton* downloadButton;
    QHBoxLayout* mainLayout;
    QVBoxLayout* infoLayout;
    QHBoxLayout* buttonLayout;
    QString fileName;
    QString fileSize;
    QString timestamp;
    QString owner;
    QString uuid;
    Mode mode;

    void setupUI();
    void setupConnections();
    QString getFileTypeAbbreviation(const QString& fileName);
};

#endif // FILE_ITEM_WIDGET_H