#include "ui/windows/login/login.h"
#include "ui/windows/received_dashboard/received_dash.h"
#include "ui/windows/sent_dashboard/sent_dash.h"
#include "./libraries/HTTPSClient.h"
#include "ui/utils/window_manager/window_manager.h"
#include <iostream>
#define SQLITE_HAS_CODEC 1
#include <QApplication>
#include <random>
#include <QLabel>
#include <QString>
#include <sstream>
#include "client_api_interactions/MakeAuthReq.h"
#include "database/database.h"
#include "database/schema.h"
#include "endpoints/endpoints.h"
#include "qrcodegen.hpp"
#include "ui/QRCodeGenerator.h"

using namespace qrcodegen;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    constexpr bool refresh_database = false;

    auto &db = Database::get();
    if (db.initialize("master key")) {
        qDebug() << "Database initialized successfully.";
    } else {
        qDebug() << "Failed to initialize database.";
        return 1;
    }

    // TODO: Debugging only
    if (refresh_database) drop_all_tables();

    init_schema();

    WindowManager::instance().showLogin();
    return QApplication::exec();
}