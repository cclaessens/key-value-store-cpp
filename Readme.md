# key-value-store-cpp

C++ implementation of key-value-store using sqlite3 and mutex locks for thread locking.


## To get it working on on a Mac with M chip:

Rebuild SQLite3 for arm64.
<br>Copy libsqlite3.a to *lib* and sqlite3.h to *include*. 
<br>Link against them in *CMakeLists.txt*.

## Test script

The test_kv_store.cpp script tests basic functionality, the speed at which 5 threads do 100 read and write operations, and checks for write skew effects.
<br>Output on Apple M1 Pro Chip (10 cores) with SSD:

```
[==========] Running 6 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 6 tests from KVStoreTest
[ RUN      ] KVStoreTest.RetrievesStoredValue
[       OK ] KVStoreTest.RetrievesStoredValue (3 ms)
[ RUN      ] KVStoreTest.UpdatesValueCorrectly
[       OK ] KVStoreTest.UpdatesValueCorrectly (1 ms)
[ RUN      ] KVStoreTest.RemovesKey
[       OK ] KVStoreTest.RemovesKey (0 ms)
[ RUN      ] KVStoreTest.MultiThreadedPerformanceTest
		        Size of value: 200000
		        Time taken: 0.131741s
[       OK ] KVStoreTest.MultiThreadedPerformanceTest (133 ms)
[ RUN      ] KVStoreTest.PersistsData
[       OK ] KVStoreTest.PersistsData (1 ms)
[ RUN      ] KVStoreTest.WriteSkew
		        Final value: initial_valuex
[       OK ] KVStoreTest.WriteSkew (1 ms)
[----------] 6 tests from KVStoreTest (141 ms total)

[----------] Global test environment tear-down
[==========] 6 tests from 1 test suite ran. (141 ms total)
[  PASSED  ] 6 tests.
```


## Comments on design choices

- **SQL**: In our experiment the slow controls software uses PostgreSQL for logging all sensor readout. Choosing something lighter for a smaller scale project brought me to sqlite3.
- **Database configuration**: I used the Write-Ahead Logging mode. From a quick research I learned that this way, changes are first written to a log and readers can still read the database file in the meantime. I also set synchronous setting to OFF. This way, speed is further increased at the risk of loosing the data from that write operation during a crash or power loss. The database itself is still persistent during a software crash.
- **multi-threading**: In the test script multiple threads are created to test parallel operations on the database. 
- **thread locks**: Multiple operations on the same instance or space in memory must be prevented. Using std:mutex to ensure thread safety. 
- **Speed**: I implemented the KeyValueStore in Python and C++. Surprisingly I found the python tests to run faster than the C++ equivalent. The reason could be the sqlite library. I had to rebuild it for my ARM chip and the built-in Python interface may just be optimized for better performance. Or the Python Global Interpreter Lock could be preventing conflicting resource access requests. 