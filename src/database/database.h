#ifndef DATABASE_INTERFACE_H
#define DATABASE_INTERFACE_H

#include <QVariantMap>
#include <QVector>
#define SQLITE_HAS_CODEC 1
#include <sqlite3.h>

#include "src/keys/secure_memory_buffer.h"

class Database {
public:
    // Singleton access
    static Database &get();

    // Initialize the database with a master key
    void initialize(const std::string &username, const std::unique_ptr<SecureMemoryBuffer> &master_key,
                    bool encrypted = true);
    bool isInitialized() const { return db != nullptr; }
    sqlite3 *getDatabase() const { return db; }
    void closeDatabase();

    void prepare_or_throw(const char *zSql, sqlite3_stmt **stmt) const;

    // Execute a prepared and bound statement (e.g. INSERT, UPDATE, DELETE, etc.)
    // Throws std::runtime_error on error or if statement is not ready
    void execute(sqlite3_stmt *stmt) const;

    // Query using a prepared and bound SELECT statement
    // Throws std::runtime_error on error or if statement is not a SELECT
    QVector<QVariantMap> query(sqlite3_stmt *stmt) const;

    void rotate_master_key(const std::unique_ptr<SecureMemoryBuffer> &old_master_key,
                           const std::unique_ptr<SecureMemoryBuffer> &new_master_key);

    static bool user_has_database(std::string username);

    bool verify_master_key(const std::unique_ptr<SecureMemoryBuffer> &master_key);

    [[nodiscard]] std::string get_username() const {
        return username;
    }

private:
    Database();
    ~Database();

    // Prevent copying
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    sqlite3 *db = nullptr;
    bool initialized = false;
    std::string username;
    std::unique_ptr<SecureMemoryBuffer> password_check;
};

#endif // DATABASE_INTERFACE_H
