all: pweb_home_agent_test

pweb_home_agent_test:		cassandra_db.o pweb_home_agent_test.o home_agent_index_cassandra_controller.o home_agent_index.o udp_connection.o io_service_pool.o dns_message_handler.o
				g++ -Wall -g obj/cassandra_db.o obj/pweb_home_agent_test.o obj/home_agent_index_cassandra_controller.o obj/home_agent_index.o obj/udp_connection.o obj/io_service_pool.o obj/dns_message_handler.o -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lleveldb -lcql -o bin/pweb_home_agent_test

home_agent_index_cassandra_controller.o: src/controllers/home_agent_index_cassandra_controller.cpp
					 g++ -Wall -c -g src/controllers/home_agent_index_cassandra_controller.cpp -o obj/home_agent_index_cassandra_controller.o

home_agent_index.o:		src/models/home_agent_index.cpp
				g++ -Wall -c -g src/models/home_agent_index.cpp -o obj/home_agent_index.o

cassandra_db.o:			src/database/cassandra_db.cpp
				g++ -Wall -c -g src/database/cassandra_db.cpp -o obj/cassandra_db.o

io_service_pool.o:		src/communication/io_service_pool.cpp
				g++ -Wall -c -g src/communication/io_service_pool.cpp -o obj/io_service_pool.o

udp_connection.o:		src/server/udp_connection.cpp
				g++ -Wall -c -g src/server/udp_connection.cpp -o obj/udp_connection.o

dns_message_handler.o:		src/protocol/dns_message_handler.cpp
				g++ -Wall -c -g src/protocol/dns_message_handler.cpp -o obj/dns_message_handler.o
				
pweb_home_agent_test.o:		src/test/pweb_home_agent_test.cpp
				g++ -Wall -c -g src/test/pweb_home_agent_test.cpp -o obj/pweb_home_agent_test.o

clean:
			rm -rf obj/*
			rm -rf bin/*


