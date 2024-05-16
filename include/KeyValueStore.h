// Created: 2024-05-15
// Description: A simple key-value store that uses sqlite3 to store key-value pairs.
// The KeyValueStore class provides methods to put, get, and delete key-value pairs.

#ifndef KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_H

#include <string>
#include <sqlite3.h>  // Ensure SQLite is included if not, include the appropriate path
#include <mutex>    // For std::mutex locking

class KeyValueStore {
public:
    // Constructor and Destructor
    KeyValueStore(const std::string& dbPath);
    ~KeyValueStore();

    // Public member functions
    void put(const std::string& key, const std::string& value);
    // get method returns only strings. should probably use a template.
    std::string get(const std::string& key);
    void remove(const std::string& key);
    void close();  // Close the database

private:
    // Helper functions
    void open(const std::string& dbPath);
    void createTableIfNotExists();

    // Member variables
    sqlite3* db;  // Pointer to SQLite database
    std::mutex db_mutex;  // Mutex for thread safety

    // sqlite statements
    sqlite3_stmt* put_stmt;
    sqlite3_stmt* get_stmt;
    sqlite3_stmt* remove_stmt;
};

#endif // KEY_VALUE_STORE_H
