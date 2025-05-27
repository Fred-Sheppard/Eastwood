#ifndef SENDFILE_H
#define SENDFILE_H

#include <QWidget>
#include "../received_dashboard/received_dash.h"

namespace Ui {
class SendFile;
}

class SendFile : public QWidget
{
    Q_OBJECT

public:
    explicit SendFile(QWidget *parent = nullptr, QWidget* receivedWindow = nullptr);
    ~SendFile();

private slots:
    void onBrowseClicked();
    void onSendClicked();
    void updateSendButtonState();
    void onReceivedButtonClicked();
    void onSentButtonClicked();
    void onSendFileButtonClicked();
    void onSettingsButtonClicked();

private:
    Ui::SendFile *ui;
    QWidget* m_receivedWindow;

    void setupUI();
    void setupConnections();
    void updateFileDetails(const QString &filePath);
};

#endif