#include "./device_register.h"
#include "ui_device_register.h"
#include "src/ui/utils/window_manager/window_manager.h"

DeviceRegister::DeviceRegister(const std::string& auth_code, const QImage& qr_code, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceRegister)
{
    ui->setupUi(this);
    setupConnections();
    displayQRCode(qr_code);
    displayAuthCode(auth_code);
}

DeviceRegister::~DeviceRegister()
{
    delete ui;
}

void DeviceRegister::setupConnections()
{
    connect(ui->backButton, &QPushButton::clicked, this, &DeviceRegister::onBackButtonClicked);
}

void DeviceRegister::displayQRCode(const QImage& qr_code)
{
    if (!qr_code.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(qr_code);
        ui->qrCodeLabel->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void DeviceRegister::displayAuthCode(const std::string& auth_code)
{
    ui->authCodeLabel->setText(QString::fromStdString(auth_code));
}

void DeviceRegister::onBackButtonClicked()
{
    WindowManager::instance().showRegister();
}