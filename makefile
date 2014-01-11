all: cassandra_driver_test

cassandra_driver_test:		cassandra_db.o cassandra_driver_test.o
				g++ -g obj/cassandra_db.o obj/cassandra_driver_test.o -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lcql -L/usr/local/lib -o bin/cassandra_driver_test

cassandra_db.o:			src/database/cassandra_db.cpp
				g++ -c -g src/database/cassandra_db.cpp -o obj/cassandra_db.o

cassandra_driver_test.o:	src/test/cassandra_driver_test.cpp
				g++ -c -g src/test/cassandra_driver_test.cpp -o obj/cassandra_driver_test.o

clean:
			rm -rf obj/*
			rm -rf bin/*


