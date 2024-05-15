# key-value-store-cpp

C++ implementation of key-value-store using sqlite3.


## Steps to get it working on my Mac:

Had to rebuild SQLite3 for arm64.
Copied libsqlite3.a to lib and sqlite3.h to include. Linking against them in CMakeLists.txt made the building work.