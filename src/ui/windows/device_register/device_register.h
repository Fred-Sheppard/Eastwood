#ifndef DEVICE_REGISTER_H
#define DEVICE_REGISTER_H

#include <QWidget>
#include <QImage>
#include <QClipboard>

QT_BEGIN_NAMESPACE
namespace Ui { class DeviceRegister; }
QT_END_NAMESPACE

class DeviceRegister : public QWidget {
    Q_OBJECT

public:
    explicit DeviceRegister(const std::string& auth_code, const QImage& qr_code, QWidget *parent = nullptr, const unsigned char* pk_device = nullptr);
    ~DeviceRegister() override;
    void displayQRCode(const QImage& qr_code) const;
    void displayAuthCode(const std::string& auth_code) const;

signals:
    void userRegistered(); // Signal emitted when background thread detects registration

private slots:
    static void onBackButtonClicked();
    void onCopyButtonClicked();

    static void onUserRegistered(); // Slot to handle login in main thread

private:
    Ui::DeviceRegister *ui;
    void setupConnections();
    std::string m_auth_code; // Store the full auth code
};

#endif // DEVICE_REGISTER_H
