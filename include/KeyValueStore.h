#ifndef KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_H

#include <string>
#include <sqlite3.h>  // Ensure SQLite is included if not, include the appropriate path
#include <mutex>

class KeyValueStore {
public:
    // Constructor and Destructor
    KeyValueStore(const std::string& dbPath);
    ~KeyValueStore();

    // Public member functions
    void put(const std::string& key, const std::string& value);
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
};

#endif // KEY_VALUE_STORE_H
