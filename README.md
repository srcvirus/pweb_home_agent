Home Agent Software for pWeb name resolution system
===================================================


### Dependencies

The home agent software uses cassandra (1.2+) as underlying
indexing storage. Details about cassandra (1.2+) installation 
can be found at:

http://www.datastax.com/documentation/cassandra/1.2/webhelp/index.html#cassandra/install/installDeb_t.html

The home agent software depends on the following libraries:

1. libboost_system-dev
2. libboost_thread-dev
3. libboost_filesystem1.53-dev
4. libboost_program-options-dev
5. libboost_test-dev
6. libboost_log
7. libboost_log_setup
8. libssl-dev
9. libssh2-1-dev
10. libcrypto++-dev
11. libcql
12. libtcmalloc 

On a Ubuntu/Debian system all these libraries (except cql and tcmalloc) can be installed from the
command line by running:
(instead of installing all the boost librarires, installing only the listed ones would
work as well)

```
sudo apt-get install libboost-all-dev libssl-dev libssh2-1-dev libcrypto++-dev
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

### Cassandra Configuration
After installing cassandra by following the instruction from their site, open cassandra's configuration file (/etc/cassandra/cassandra-env.sh). Under the "seeds" section add the IP addresses 132.206.206.134 as a bootstrap node to join. 
