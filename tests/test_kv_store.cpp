#include "gtest/gtest.h"
#include "KeyValueStore.h"

class KVStoreTest : public ::testing::Test {
protected:
    KeyValueStore* store;

    void SetUp() override {
        store = new KeyValueStore("test.db");
        store->put("testkey", "testvalue");
    }

    void TearDown() override {
        delete store;
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
