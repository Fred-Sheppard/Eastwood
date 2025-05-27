#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <QObject>

// Forward declarations to reduce includes
class Received;
class Sent;
class SendFile;
class Settings;  // Add this forward declaration

class WindowManager : public QObject
{
    Q_OBJECT
public:
    static WindowManager& instance();

    void showReceived();
    void showSent();
    void showSendFile();
    void showSettings();  // Add this method declaration

    // Add cleanup method
    void cleanup();

signals:
    void windowShown(const QString& windowName);

protected:
    WindowManager();
    virtual ~WindowManager();

private:
    // Delete copy constructor and assignment operator
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    // Template function to handle different window types
    template<typename T>
    void deleteWindow(T*& window) {
        if (window) {
            window->hide();
            window->deleteLater();
            window = nullptr;
        }
    }

    Received* m_received;
    Sent* m_sent;
    SendFile* m_sendFile;
    Settings* m_settings;  // Add this member variable
};

#endif // WINDOW_MANAGER_H