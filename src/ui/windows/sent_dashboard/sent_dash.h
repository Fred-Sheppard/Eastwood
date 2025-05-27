#ifndef SENT_DASH_H
#define SENT_DASH_H

#include <QWidget>
#include <QListWidgetItem>
#include "../../utils/file_item_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Sent; }
QT_END_NAMESPACE

class Sent : public QWidget {
    Q_OBJECT

public:
    explicit Sent(QWidget *parent = nullptr);
    ~Sent() override;

private slots:
    void onSendButtonClicked();
    void onFileItemClicked(FileItemWidget* widget);
    void onRevokeAccessClicked(FileItemWidget* widget);
    void onDeleteFileClicked(FileItemWidget* widget);
    void onReceivedButtonClicked();
    void onSettingsButtonClicked();
    void refreshFileList();

private:
    Ui::Sent *ui;
    void setupConnections();
    void setupFileList();
    void showFileMetadata(FileItemWidget* widget);
    void sendFileToUser(const QString& username, const QString& fileId);
    void addFileItem(const QString& fileName, 
                    const QString& fileSize, 
                    const QString& timestamp,
                    const QString& owner);
};

#endif // SENT_DASH_H