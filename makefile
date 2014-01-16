all: cassandra_driver_test

cassandra_driver_test:		cassandra_db.o cassandra_driver_test.o home_agent_index_cassandra_controller.o home_agent_index.o home_agent_server.o io_service_pool.o
				g++ -g obj/cassandra_db.o obj/cassandra_driver_test.o obj/home_agent_index_cassandra_controller.o obj/home_agent_index.o obj/home_agent_server.o obj/io_service_pool.o -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lcql -o bin/cassandra_driver_test

home_agent_index_cassandra_controller.o: src/controllers/home_agent_index_cassandra_controller.cpp
					 g++ -c -g src/controllers/home_agent_index_cassandra_controller.cpp -o obj/home_agent_index_cassandra_controller.o

home_agent_index.o:		src/models/home_agent_index.cpp
				g++ -c -g src/models/home_agent_index.cpp -o obj/home_agent_index.o

cassandra_db.o:			src/database/cassandra_db.cpp
				g++ -c -g src/database/cassandra_db.cpp -o obj/cassandra_db.o

home_agent_server.o:		src/server/home_agent_server.cpp
				g++ -c -g src/server/home_agent_server.cpp -o obj/home_agent_server.o

io_service_pool.o:		src/communication/io_service_pool.cpp
				g++ -c -g src/communication/io_service_pool.cpp -o obj/io_service_pool.o

cassandra_driver_test.o:	src/test/cassandra_driver_test.cpp
				g++ -c -g src/test/cassandra_driver_test.cpp -o obj/cassandra_driver_test.o

clean:
			rm -rf obj/*
			rm -rf bin/*


