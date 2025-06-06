#ifndef RECEIVED_DASH_H
#define RECEIVED_DASH_H

#include <QWidget>
#include <QListWidgetItem>
#include "src/ui/utils/file_item_widget/file_item_widget.h"
#include "src/ui/utils/window_manager/window_manager.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Received; }
QT_END_NAMESPACE

class Received : public QWidget {
    Q_OBJECT

public:
    explicit Received(QWidget *parent = nullptr, QWidget* sendFileWindow = nullptr);
    ~Received() override;

private slots:
    void onSendButtonClicked();
    void onFileItemClicked(const FileItemWidget* widget);
    void onDownloadFileClicked(FileItemWidget* widget);
    void refreshFileList();
    void onReceivedButtonClicked();
    void onRefreshButtonClicked();
    void handleRefreshSpinner();
    void onShowAuthCodeButtonClicked();

private:
    Ui::Received *ui;
    QWidget* m_sendFileWindow;
    QTimer* m_refreshSpinnerTimer;
    int m_spinnerAngle;
    void setupConnections();
    void setupFileList() const;
    void showFileMetadata(const FileItemWidget* widget);
    static void sendFileToUser(const QString& username, const QString& fileId);
    void addFileItem(const QString& fileName, 
                    const QString& fileSize, 
                    const QString& timestamp,
                    const QString& owner,
                    std::string uuid,
                    std::string mime_type);
};

#endif // RECEIVED_DASH_H