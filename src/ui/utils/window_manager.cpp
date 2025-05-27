#include "window_manager.h"
#include "../windows/received_dashboard/received_dash.h"
#include "../windows/sent_dashboard/sent_dash.h"
#include "../windows/send_file/send_file.h"
#include "../windows/settings/settings.h"

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
    if (m_received) {
        m_received->show();
    } else {
        m_received = new Received();
        m_received->setAttribute(Qt::WA_DeleteOnClose);
        m_received->show();
    }
}

void WindowManager::showSent()
{
    if (m_sent) {
        m_sent->show();
    } else {
        m_sent = new Sent();
        m_sent->setAttribute(Qt::WA_DeleteOnClose);
        m_sent->show();
    }
}

void WindowManager::showSendFile()
{
    if (m_sendFile) {
        m_sendFile->show();
    } else {
        m_sendFile = new SendFile();
        m_sendFile->setAttribute(Qt::WA_DeleteOnClose);
        m_sendFile->show();
    }
} 

void WindowManager::showSettings()
{ 
    if (m_settings) {
        m_settings->show();
    } else {
        m_settings = new Settings();
        m_settings->setAttribute(Qt::WA_DeleteOnClose);
        m_settings->show();
    }
}