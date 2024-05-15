#include <memory>
#include "gtest/gtest.h"
#include "KeyValueStore.h"

class KVStoreTest : public ::testing::Test {
protected:
    // Create a shared pointer to the KeyValueStore object to not copy the object
    std::shared_ptr<KeyValueStore> store;

    void SetUp() override {
        store = std::make_shared<KeyValueStore>("test.db");
        store->put("testkey", "testvalue");
    }

    void TearDown() override {
        store.reset();
    }
};

TEST_F(KVStoreTest, RetrievesStoredValue) {
    ASSERT_EQ("testvalue", store->get("testkey"));
}

TEST_F(KVStoreTest, UpdatesValueCorrectly) {
    store->put("testkey", "newvalue");
    ASSERT_EQ("newvalue", store->get("testkey"));
}

TEST_F(KVStoreTest, RemovesKey) {
    store->remove("testkey");
    ASSERT_TRUE(store->get("testkey").empty());
}

// Test speed of storing and saving values that are 100kB in size
TEST_F(KVStoreTest, PerformanceTest) {
    std::string key = "largekey";
    std::string value(100000, 'a');
    store->put(key, value);
    ASSERT_EQ(value, store->get(key));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
