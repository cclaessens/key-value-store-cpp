#include "KeyValueStore.h"
#include <stdexcept>

// Constructor
KeyValueStore::KeyValueStore(const std::string& dbPath) {
    open(dbPath);
    createTableIfNotExists();
}

// Destructor
KeyValueStore::~KeyValueStore() {
    sqlite3_close(db);
}

// Open the SQLite database
void KeyValueStore::open(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }
}

// Create the key-value store table if it does not exist
void KeyValueStore::createTableIfNotExists() {
    const char* sqlCreateTable = "CREATE TABLE IF NOT EXISTS store (key TEXT PRIMARY KEY, value TEXT);";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sqlCreateTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to create table: " + std::string(errMsg));
    }
}

// Insert or update a value in the store
void KeyValueStore::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    const char* sqlInsert = "REPLACE INTO store (key, value) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);
}

// Retrieve a value from the store
std::string KeyValueStore::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(db_mutex);
    const char* sqlSelect = "SELECT value FROM store WHERE key = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string value(result);
        sqlite3_finalize(stmt);
        return value;
    }
    sqlite3_finalize(stmt);
    return "";  // Return an empty string if not found
}

// Delete a key-value pair from the store
void KeyValueStore::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(db_mutex);
    const char* sqlDelete = "DELETE FROM store WHERE key = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sqlDelete, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);
}