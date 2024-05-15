cmake_minimum_required(VERSION 3.10)
project(KeyValueStore)

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