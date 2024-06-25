#include <memory>
#include "gtest/gtest.h"
#include "KeyValueStore.h"
#include <thread>

class KVStoreTest : public ::testing::Test {
protected:
    // Create a shared pointer to the KeyValueStore object to not copy the object (mutex is not copyable)
    std::shared_ptr<KeyValueStore> store;

    // Set up store and put a value in it
    void SetUp() override {
        store = std::make_shared<KeyValueStore>("test.db");
        store->put("testkey", "testvalue");
    }

    // Reset the store
    void TearDown() override {
        store.reset();
    }
};

// Test getting a value
TEST_F(KVStoreTest, RetrievesStoredValue) {
    ASSERT_EQ("testvalue", store->get("testkey"));
}

// Test updating a value
TEST_F(KVStoreTest, UpdatesValueCorrectly) {
    store->put("testkey", "newvalue");
    ASSERT_EQ("newvalue", store->get("testkey"));
}

// Test removing a key
TEST_F(KVStoreTest, RemovesKey) {
    store->remove("testkey");
    ASSERT_TRUE(store->get("testkey").empty());
}

// Test speed of storing and saving values that are >100kB in size
TEST_F(KVStoreTest, MultiThreadedPerformanceTest) {
    std::string key = "largekey";
    std::string value(200000, 'a');
    // print size of value
    std::cout << "\t\tSize of value: " << value.size() << std::endl;

    // start timing
    auto start = std::chrono::high_resolution_clock::now();
    // Start 5 threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.push_back(std::thread([this, &key, &value]() {
            // Each thread stores and retrieves the value 100 times
            for (int j = 0; j < 100; j++) {
                store->put(key, value);
                store->get(key);
            }
        }));
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    // end timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    // print time taken
    std::cout << "\t\tTime taken: " << elapsed_seconds.count() << "s" << std::endl;
}
// Test persistence: Does the testkey persist after the shared pointer is reset?
TEST_F(KVStoreTest, PersistsData) {
    store.reset();
    // Create a new KeyValueStore object with the same database file
    store = std::make_shared<KeyValueStore>("test.db");
    // Check if the value is still there
    ASSERT_EQ("testvalue", store->get("testkey"));
}

// Test write skew
TEST_F(KVStoreTest, WriteSkew) {
    std::string key = "write_skew";
    store->put(key, "initial_value");
    std::vector<std::thread> threads;
    for (int i = 0; i < 15; i++) {
        threads.push_back(std::thread([this, &key]() {
            std::string value = store->get(key);

            // If the value is the initial value, update it
            // If multiple threads read the intial value, they will add multiple x's to the value
            if (value == "initial_value") {
                store->put(key, value + "x");
            }
        }));
    }
    for (auto& thread : threads) {
        thread.join();
    }
    // print final value
    std::cout << "\t\tFinal value: " << store->get(key) << std::endl;
    // Check if the final value is the initial value with only one x appended
    ASSERT_EQ("initial_valuex", store->get(key));
}


int main(int argc, char **argv) {
    // Using Google Test
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
