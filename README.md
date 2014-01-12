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
3. libssl-dev
4. libcrypto++-dev
5. libcql

On a Ubuntu/Debian system all these libraries (except cql) can be installed from the
command line by running:

```
sudo apt-get install libboost-system-dev libboost-thread-dev libssl-dev libcrypto++-dev 
```

To install libcql download the Datastax C++ driver from github by running:
(the cpp-driver requires cmake. To install cmake run sudo apt-get install cmake)

```
git clone https://github.com/datastax/cpp-driver.git
cd cpp-driver
cmake . && make && make cql_demo && make cql_test && make test && make install
```
