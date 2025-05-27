#ifndef RECEIVED_DASH_H
#define RECEIVED_DASH_H

#include <QWidget>
#include <QListWidgetItem>
#include "../../utils/file_item_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Received; }
QT_END_NAMESPACE

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
    void onSentButtonClicked();
    void onSettingsButtonClicked();
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

#endif // RECEIVED_DASH_H