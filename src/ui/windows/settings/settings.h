#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include "src/ui/utils/camera_functionality/camera_functionality.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Settings; }
QT_END_NAMESPACE

class Settings : public QWidget {
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;

private slots:
    void onWindowShown(const QString& windowName) const;
    void onReceivedButtonClicked();
    void onSentButtonClicked();
    void onSendFileButtonClicked();
    void onLogoutButtonClicked();
    void validatePassphrase();
    void onPassphraseCancelClicked();
    void onPassphraseSaveClicked();
    void onAuthCancelClicked();
    void onAuthVerifyClicked();
    void onSettingsButtonClicked();
    void onScanQRButtonClicked();
    void onRefreshDevicesClicked();
    void updateDeviceList();

private:
    void setupConnections();
    void navigateTo(QWidget* newWindow);
    void createDeviceBox(const std::string& deviceName);

    Ui::Settings *ui;
    CameraFunctionality* m_cameraFunctionality;
};

#endif // SETTINGS_H