#include "window_manager.h"
#include "../../windows/received_dashboard/received_dash.h"
#include "../../windows/sent_dashboard/sent_dash.h"
#include "../../windows/send_file/send_file.h"
#include "../../windows/settings/settings.h"

WindowManager& WindowManager::instance()
{
    static WindowManager instance;
    return instance;
}

WindowManager::WindowManager()
    : m_received(nullptr)
    , m_sent(nullptr)
    , m_sendFile(nullptr)
    , m_settings(nullptr)
{
}

WindowManager::~WindowManager()
{
    cleanup();
}

void WindowManager::cleanup()
{
    deleteWindow(m_received);
    deleteWindow(m_sent);
    deleteWindow(m_sendFile);
    deleteWindow(m_settings);
}

void WindowManager::showReceived()
{
    if (!m_received) {
        m_received = new Received();
        connect(m_received, &Received::destroyed, this, [this]() {
            m_received = nullptr;
        });
    }
    m_received->show();
    emit windowShown("receivedButton");
}

void WindowManager::showSent()
{
    if (!m_sent) {
        m_sent = new Sent();
        connect(m_sent, &Sent::destroyed, this, [this]() {
            m_sent = nullptr;
        });
    }
    m_sent->show();
    emit windowShown("sentButton");
}

void WindowManager::showSendFile()
{
    if (!m_sendFile) {
        m_sendFile = new SendFile();
        connect(m_sendFile, &SendFile::destroyed, this, [this]() {
            m_sendFile = nullptr;
        });
    }
    m_sendFile->show();
    emit windowShown("sendFileButton");
}

void WindowManager::showSettings()
{
    if (!m_settings) {
        m_settings = new Settings();
        connect(m_settings, &Settings::destroyed, this, [this]() {
            m_settings = nullptr;
        });
    }
    m_settings->show();
    emit windowShown("settingsButton");
}

void WindowManager::showLogin() {
    // Close all existing wi
}