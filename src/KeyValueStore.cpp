// Created: 2024-05-15

#include "KeyValueStore.h"
#include <stdexcept> // for throwing exceptions

// Constructor
KeyValueStore::KeyValueStore(const std::string& dbPath) {
    // setup the database
    open(dbPath);
    createTableIfNotExists();
}

// Destructor
KeyValueStore::~KeyValueStore() {
    // first cleanup the prepared statements
    if (put_stmt) {
        sqlite3_finalize(put_stmt);
    }
    if (get_stmt) {
        sqlite3_finalize(get_stmt);
    }
    if (remove_stmt) {
        sqlite3_finalize(remove_stmt);
    }
    // close the database
    sqlite3_close(db);
}

// Open the database
void KeyValueStore::open(const std::string& dbPath) {
    // if successful, db will point to the database
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }

    // Enable WAL mode for better performance
    const char* sqlWAL = "PRAGMA journal_mode=WAL;";
    char* errMsg = nullptr;
    // in case of error, sqlite3_exec will modify errMsg to point to the error message
    if (sqlite3_exec(db, sqlWAL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to enable WAL mode: " + std::string(errMsg));
    }
    // fsync off to improve write performance
    const char* sqlFsync = "PRAGMA synchronous=OFF;";
    if (sqlite3_exec(db, sqlFsync, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to disable fsync: " + std::string(errMsg));
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
// void KeyValueStore::put(const std::string& key, const std::string& value) {
//     std::lock_guard<std::mutex> lock(db_mutex);
//     const char* sqlInsert = "REPLACE INTO store (key, value) VALUES (?, ?);";
//     sqlite3_stmt* stmt;
//     if (sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, nullptr) != SQLITE_OK) {
//         throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
//     }
//     sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
//     sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
//     if (sqlite3_step(stmt) != SQLITE_DONE) {
//         sqlite3_finalize(stmt);
//         throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(db)));
//     }
//     sqlite3_finalize(stmt);
// }

// This version of put uses prepared statements to try and improve performance
// It didn't actually make the tests run faster. Maybe the overhead of preparing the statement balances the benefits.
void KeyValueStore::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!put_stmt) {
            const char* sqlInsert = "REPLACE INTO store (key, value) VALUES (?, ?);";
            if (sqlite3_prepare_v2(db, sqlInsert, -1, &put_stmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("Failed to prepare statement");
            }
        }
        // replace the ? placeholders with the actual values
        sqlite3_bind_text(put_stmt, 1, key.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(put_stmt, 2, value.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(put_stmt) != SQLITE_DONE) {
            // check why SQLITE_DONE is not True
            if (SQLITE_BUSY) {
                throw std::runtime_error("Failed to execute statement. The database file is locked: " + std::string(sqlite3_errmsg(db)));
            }
            throw std::runtime_error("Failed to execute statement");
        }
        // reset statement so it can be used again
        sqlite3_reset(put_stmt);
}


// Retrieve a value from the store
// std::string KeyValueStore::get(const std::string& key) {
//     std::lock_guard<std::mutex> lock(db_mutex);
//     const char* sqlSelect = "SELECT value FROM store WHERE key = ?;";
//     sqlite3_stmt* stmt;
//     if (sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, nullptr) != SQLITE_OK) {
//         throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
//     }
//     sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
//     if (sqlite3_step(stmt) == SQLITE_ROW) {
//         const char* result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
//         std::string value(result);
//         sqlite3_finalize(stmt);
//         return value;
//     }
//     sqlite3_finalize(stmt);
//     return "";  // Return an empty string if not found
// }
std::string KeyValueStore::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!get_stmt) {
        const char* sqlSelect = "SELECT value FROM store WHERE key = ?;";
        if (sqlite3_prepare_v2(db, sqlSelect, -1, &get_stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }
    }
    sqlite3_bind_text(get_stmt, 1, key.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(get_stmt) == SQLITE_ROW) {
        const char* result = reinterpret_cast<const char*>(sqlite3_column_text(get_stmt, 0));
        std::string value(result);
        sqlite3_reset(get_stmt);
        return value;
    }
    sqlite3_reset(get_stmt);
    return "";  // Return an empty string if not found
}

// Delete a key-value pair from the store
// void KeyValueStore::remove(const std::string& key) {
//     std::lock_guard<std::mutex> lock(db_mutex);
//     const char* sqlDelete = "DELETE FROM store WHERE key = ?;";
//     sqlite3_stmt* stmt;
//     if (sqlite3_prepare_v2(db, sqlDelete, -1, &stmt, nullptr) != SQLITE_OK) {
//         throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
//     }
//     sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
//     if (sqlite3_step(stmt) != SQLITE_DONE) {
//         sqlite3_finalize(stmt);
//         throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(db)));
//     }
//     sqlite3_finalize(stmt);
// }
void KeyValueStore::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (!remove_stmt) {
        const char* sqlDelete = "DELETE FROM store WHERE key = ?;";
        if (sqlite3_prepare_v2(db, sqlDelete, -1, &remove_stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }
    }
    sqlite3_bind_text(remove_stmt, 1, key.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(remove_stmt) != SQLITE_DONE) {
        throw std::runtime_error("Failed to execute statement");
    }
    sqlite3_reset(remove_stmt);
}