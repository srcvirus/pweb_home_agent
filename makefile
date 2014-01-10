#g++ cassandra_db.o main.o -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lcql -L/usr/local/lib -o test


all: cassandra_driver_test

cassandra_driver_test:		cassandra_db.o cassandra_driver_test.o
				g++ obj/cassandra_db.o obj/cassandra_driver_test.o -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lcql -L/usr/local/lib -o bin/cassandra_driver_test

cassandra_db.o:			src/cassandra_db.cpp
				g++ -c src/cassandra_db.cpp -o obj/cassandra_db.o

cassandra_driver_test.o:	src/cassandra_driver_test.cpp
				g++ -c src/cassandra_driver_test.cpp -o obj/cassandra_driver_test.o

clean:
			rm -rf obj/*
			rm -rf bin/*


