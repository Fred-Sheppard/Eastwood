#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QString>
#define SQLITE_HAS_CODEC 1
#include <sqlite3.h>

class DatabaseManager {
public:
    static DatabaseManager& getInstance();
    
    bool initialize(const QString& masterKey);
    bool isInitialized() const { return db != nullptr; }
    sqlite3* getDatabase() const { return db; }
    void closeDatabase();

private:
    DatabaseManager() = default;
    ~DatabaseManager();
    
    // Prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    sqlite3* db = nullptr;
    bool initialized = false;
};

#endif // DATABASE_MANAGER_H 