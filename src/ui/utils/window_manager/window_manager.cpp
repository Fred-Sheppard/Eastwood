#include "window_manager.h"
#include "../../windows/received_dashboard/received_dash.h"
#include "../../windows/sent_dashboard/sent_dash.h"
#include "../../windows/send_file/send_file.h"
#include "../../windows/settings/settings.h"
#include "../../windows/login/login.h"
#include "../../windows/register/register.h"
#include "../../windows/device_register/device_register.h"
#include <QImage>

WindowManager& WindowManager::instance()
{
    static WindowManager instance;
    return instance;
}

WindowManager::WindowManager()
{
}

WindowManager::~WindowManager()
{
    cleanup();
}

void WindowManager::cleanup()
{
    // Close all windows first
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    // Then clean up individual pointers
    deleteWindow(m_received);
    deleteWindow(m_sent);
    deleteWindow(m_sendFile);
    deleteWindow(m_settings);
    deleteWindow(m_login);
    deleteWindow(m_register);
    deleteWindow(m_deviceRegister);
}

void WindowManager::showReceived()
{
    // Close all existing windows
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    if (m_received.isNull()) {
        m_received = new Received();
        m_received->setAttribute(Qt::WA_DeleteOnClose);
        m_windows.append(QPointer<QWidget>(m_received));
        connect(m_received, &Received::destroyed, this, [this]() {
            m_received = nullptr;
            m_windows.removeOne(QPointer<QWidget>(m_received));
        });
    }
    m_received->show();
    emit windowShown("receivedButton");
}

void WindowManager::showSent()
{
    // Close all existing windows
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    if (m_sent.isNull()) {
        m_sent = new Sent();
        m_sent->setAttribute(Qt::WA_DeleteOnClose);
        m_windows.append(QPointer<QWidget>(m_sent));
        connect(m_sent, &Sent::destroyed, this, [this]() {
            m_sent = nullptr;
            m_windows.removeOne(QPointer<QWidget>(m_sent));
        });
    }
    m_sent->show();
    emit windowShown("sentButton");
}

void WindowManager::showSendFile()
{
    // Close all existing windows
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    if (m_sendFile.isNull()) {
        m_sendFile = new SendFile();
        m_sendFile->setAttribute(Qt::WA_DeleteOnClose);
        m_windows.append(QPointer<QWidget>(m_sendFile));
        connect(m_sendFile, &SendFile::destroyed, this, [this]() {
            m_sendFile = nullptr;
            m_windows.removeOne(QPointer<QWidget>(m_sendFile));
        });
    }
    m_sendFile->show();
    emit windowShown("sendFileButton");
}

void WindowManager::showSettings()
{
    // Close all existing windows
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    if (m_settings.isNull()) {
        m_settings = new Settings();
        m_settings->setAttribute(Qt::WA_DeleteOnClose);
        m_windows.append(QPointer<QWidget>(m_settings));
        connect(m_settings, &Settings::destroyed, this, [this]() {
            m_settings = nullptr;
            m_windows.removeOne(QPointer<QWidget>(m_settings));
        });
    }
    m_settings->show();
    emit windowShown("settingsButton");
}

void WindowManager::showLogin()
{
    // Close all existing windows
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    if (m_login.isNull()) {
        m_login = new Login();
        m_login->setAttribute(Qt::WA_DeleteOnClose);
        m_windows.append(QPointer<QWidget>(m_login));
        connect(m_login, &Login::destroyed, this, [this]() {
            m_login = nullptr;
            m_windows.removeOne(QPointer<QWidget>(m_login));
        });
    }
    m_login->show();
    emit windowShown("loginButton");
}

void WindowManager::showRegister()
{
    // Close all existing windows
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    if (m_register.isNull()) {
        m_register = new Register();
        m_register->setAttribute(Qt::WA_DeleteOnClose);
        m_windows.append(QPointer<QWidget>(m_register));
        connect(m_register, &Register::destroyed, this, [this]() {
            m_register = nullptr;
            m_windows.removeOne(QPointer<QWidget>(m_register));
        });
    }
    m_register->show();
    emit windowShown("registerButton");
}

void WindowManager::showDeviceRegister(const std::string& auth_code, const QImage& qr_code)
{
    // Close all existing windows
    for (const QPointer<QWidget>& window : m_windows) {
        if (!window.isNull()) {
            window->close();
        }
    }
    m_windows.clear();

    if (m_deviceRegister.isNull()) {
        m_deviceRegister = new DeviceRegister(auth_code, qr_code);
        m_deviceRegister->setAttribute(Qt::WA_DeleteOnClose);
        m_windows.append(QPointer<QWidget>(m_deviceRegister));
        connect(m_deviceRegister, &DeviceRegister::destroyed, this, [this]() {
            m_deviceRegister = nullptr;
            m_windows.removeOne(QPointer<QWidget>(m_deviceRegister));
        });
    }
    
    m_deviceRegister->show();
    emit windowShown("deviceRegisterButton");
}