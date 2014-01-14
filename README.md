Home Agent Software for pWeb name resolution system
===================================================


### Dependencies

The home agent software uses cassandra (1.2+) as underlying
indexing storage. Details about cassandra (1.2+) installation can
be found at:

http://www.datastax.com/documentation/cassandra/1.2/webhelp/index.html#cassandra/install/installDeb_t.html

The home agent software depends on the following libraries:

1. libboost_system-dev
2. libboost_thread-dev
3. libboost_filesystem1.53-dev
4. libboost_program-options-dev
5. libboost_test-dev
6. libssl-dev
7. libcrypto++-dev
8. libcql
 

On a Ubuntu/Debian system all these libraries (except cql) can be installed from the
command line by running:

```
sudo apt-get install libboost-system-dev libboost-thread-dev libboost-filesystem1.53-dev  libssl-dev libcrypto++-dev libboost-test-dev libboost-program-options-dev
```

To install libcql download the Datastax C++ driver from github by running:
(the cpp-driver requires cmake. To install cmake run sudo apt-get install cmake)

```
git clone https://github.com/datastax/cpp-driver.git
cd cpp-driver
cmake . && make && make cql_demo && make cql_test && make test && make install
```
More details along with example usage of libcql can be found in its github page:
https://github.com/datastax/cpp-driver 
