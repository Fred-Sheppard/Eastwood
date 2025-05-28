#include "ui/windows/login/login.h"
#include "ui/windows/received_dashboard/received_dash.h"
#include "ui/windows/sent_dashboard/sent_dash.h"
#include "./libraries/HTTPSClient.h"
#include "ui/utils/window_manager/window_manager.h"
#include <iostream>
#define SQLITE_HAS_CODEC 1
#include <QFile>
#include <QApplication>
#include "auth/register_user/register_user.h"
#include "database/database.h"
#include "database/schema.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    constexpr bool encrypted = false;
    auto &db = Database::get();
    if (db.initialize("master key", encrypted)) {
        std::cout << "Database initialized successfully." << std::endl;
    } else {
        std::cout << "Failed to initialize database." << std::endl;
        return 1;
    }

    drop_all_tables();
    init_schema();

    WindowManager::instance().showLogin();
    return app.exec();
}
