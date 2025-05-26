#ifndef RECEIVED_H
#define RECEIVED_H

#include <QWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class Received; }
QT_END_NAMESPACE

// Custom widget for file items
class FileItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit FileItemWidget(const QString& fileName, 
                          const QString& fileSize, 
                          const QString& timestamp,
                          const QString& owner,
                          QWidget* parent = nullptr);

signals:
    void revokeAccessClicked(FileItemWidget* widget);
    void deleteFileClicked(FileItemWidget* widget);
    void fileClicked(FileItemWidget* widget);

private:
    QLabel* fileNameLabel;
    QLabel* detailsLabel;
    QPushButton* revokeButton;
    QPushButton* deleteButton;
    QString fileName;
    QString fileSize;
    QString timestamp;
    QString owner;

    void setupUI();
    void setupConnections();
    friend class Received;
};

class Received : public QWidget {
    Q_OBJECT

public:
    explicit Received(QWidget *parent = nullptr);
    ~Received() override;

private slots:
    void onSendButtonClicked();
    void onFileItemClicked(FileItemWidget* widget);
    void onRevokeAccessClicked(FileItemWidget* widget);
    void onDeleteFileClicked(FileItemWidget* widget);
    void onSentDashboardClicked();
    void refreshFileList();

private:
    Ui::Received *ui;
    void setupConnections();
    void setupFileList();
    void showFileMetadata(FileItemWidget* widget);
    void sendFileToUser(const QString& username, const QString& fileId);
    void addFileItem(const QString& fileName, 
                    const QString& fileSize, 
                    const QString& timestamp,
                    const QString& owner);
};

#endif // RECEIVED_H