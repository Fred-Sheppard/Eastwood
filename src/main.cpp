#include "./libraries/HTTPSClient.h"
#include "ui/utils/window_manager/window_manager.h"
#define SQLITE_HAS_CODEC 1
#include <QApplication>
#include <random>
#include <thread>
#include <iostream>
#include <QImage>
#include "key_exchange/utils.h"

#include "database/schema.h"
#include "endpoints/endpoints.h"
#include "sessions/RatchetSessionManager.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    if (sodium_init() < 0) {
        throw std::runtime_error("Libsodium initialization failed");
    }

    // Show device register with dummy data
    std::string dummy_auth_code = "ABCD1234EFGH5678IJKL9012MNOP3456QRST7890WXYZ";
    QImage dummy_qr_code(200, 200, QImage::Format_RGB32);
    dummy_qr_code.fill(Qt::white);
    
    WindowManager::instance().showLogin();
    return app.exec();
}
