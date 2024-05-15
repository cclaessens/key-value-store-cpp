cmake_minimum_required(VERSION 3.10)
project(KeyValueStore)

# Enable CMake's testing functionality
enable_testing()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(KeyValueStore src/main.cpp src/KeyValueStore.cpp)

# Include directories
target_include_directories(KeyValueStore PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include")

# Path to the directory containing sqlite3.h
set(SQLITE3_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# Path to the directory containing libsqlite3.a
set(SQLITE3_LIBRARY ${PROJECT_SOURCE_DIR}/lib/libsqlite3.a)

# Include SQLite3 header files
target_include_directories(KeyValueStore PUBLIC ${SQLITE3_INCLUDE_DIR})

# Link to SQLite3
target_link_libraries(KeyValueStore ${SQLITE3_LIBRARY})

# If not using SQLite3 rebuild, uncomment the following lines
# Link any necessary libraries (e.g., SQLite3 if used)
#find_package(SQLite3)
#if(SQLITE3_FOUND)
#  target_include_directories(KeyValueStore PUBLIC ${SQLITE3_INCLUDE_DIRS})
#  target_link_libraries(KeyValueStore ${SQLITE3_LIBRARIES})
#endif()


# For testing
# Include FetchContent module to fetch dependencies
include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Now declare your test executable
add_executable(KVStoreTest tests/test_kv_store.cpp)

# Link Google Test and your project library
target_link_libraries(KVStoreTest gtest_main KeyValueStoreLib ${SQLITE3_LIBRARY})

# Include Google Test's includes
target_include_directories(KVStoreTest PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
)

# Add tests to CMake's test system
include(GoogleTest)
gtest_discover_tests(KVStoreTest)

# Create a library for the KeyValueStore code
add_library(KeyValueStoreLib STATIC src/KeyValueStore.cpp)
target_include_directories(KeyValueStoreLib PUBLIC "${PROJECT_SOURCE_DIR}/include")

# Link library to the main application
target_link_libraries(KeyValueStore KeyValueStoreLib)

# Link library to the test executable
target_link_libraries(KVStoreTest gtest_main KeyValueStoreLib)
