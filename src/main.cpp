#include "KeyValueStore.h"
#include <iostream>

int main() {

    std::shared_ptr<KeyValueStore> store;

    store = std::make_shared<KeyValueStore>("kvstore.db");
    std::cout << "Opened database " << std::endl;

    store->put("name", "John Doe");
    std::cout << "Stored name: " << store->get("name") << std::endl;

    store->put("name", "Jane Doe");
    std::cout << "Updated name: " << store->get("name") << std::endl;

    store->remove("name");
    std::cout << "Deleted name, new value: '" << store->get("name") << "'" << std::endl;

    return 0;
}