#include "database_manager.h"

#include <iostream>
#include <QDir>
#include <QStandardPaths>

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::initialize(const QString& masterKey) {
    if (initialized) {
        return true;
    }

    // Get the application data directory
    const QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    const QString dbPath = dataPath + "/encrypted.db";
    std::cerr << "DB saved at " << dbPath.toStdString() << std::endl;

    // Open the database
    int rc = sqlite3_open(dbPath.toUtf8().constData(), &db);
    if (rc != SQLITE_OK) {
        qDebug() << "Can't open database:" << sqlite3_errmsg(db);
        return false;
    }

    // Set encryption key
    // TODO check this
    rc = sqlite3_key(db, masterKey.toUtf8().constData(), masterKey.length());
    if (rc != SQLITE_OK) {
        qDebug() << "Can't set encryption key:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Verify the database is accessible by running a test query
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, "SELECT 1;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        qDebug() << "Database verification failed:" << errMsg;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    initialized = true;
    return true;
}

void DatabaseManager::closeDatabase() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
        initialized = false;
    }
} 