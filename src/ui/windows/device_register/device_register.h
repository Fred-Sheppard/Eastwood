#ifndef DEVICE_REGISTER_H
#define DEVICE_REGISTER_H

#include <QWidget>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class DeviceRegister; }
QT_END_NAMESPACE

class DeviceRegister : public QWidget {
    Q_OBJECT

public:
    explicit DeviceRegister(const std::string& auth_code, const QImage& qr_code, QWidget *parent = nullptr);
    ~DeviceRegister() override;
    void displayQRCode(const QImage& qr_code);
    void displayAuthCode(const std::string& auth_code);

private slots:
    void onBackButtonClicked();

private:
    Ui::DeviceRegister *ui;
    void setupConnections();
};

#endif // DEVICE_REGISTER_H
