#include <stdio.h>
#include <boost/asio.hpp>

#include "../database/cassandra_db.h"
#include "../interfaces/database_driver.h"
#include "../controllers/home_agent_index_cassandra_controller.h"
#include "../models/home_agent_index.h"
#include "../protocol/protocol_helper.h"
#include "../server/home_agent_server.h"


#include <pthread.h>
#include <unistd.h>

//#include <rocksdb/db.h>
#include <leveldb/db.h>

#define N_THREADS 8
#define N_KEYS 3
#define N_REPS 1000

timeval start, end, elapsed;
boost::shared_ptr <CassandraDBDriver> database;

/*boost::asio::io_service ioService;
boost::asio::ip::udp::endpoint remoteEndpoint;
boost::array <char, 1024> buffer;
boost::asio::ip::udp::socket udpListenSocket(ioService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 1150));
*/

int timeval_subtract (struct timeval *result, struct timeval *x,struct timeval  *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

void *run_query(void* args)
{
	boost::shared_ptr <CassandraDBDriver> dbDriver = *((boost::shared_ptr <CassandraDBDriver>*)args);
	string key_set[] = {"mypc0", "mypc1", "mypc2"};

	HomeAgentIndexCassandraController haIndexController(dbDriver);

	for(int j = 0; j < N_REPS; j++)
	{
		for(int i = 0; i < N_KEYS; i++)
		{
			boost::shared_ptr <HomeAgentIndex> haIndexObj = haIndexController.getHomeAgentIndex(key_set[i]);
			//haIndexObj->printShort();
		}
	}
	printf("Finished query execution\n");
}

void test_protocol_helper()
{
	printf("Testing protocol helpers\n");

	char *buf = new char[100];

	int testInt = 122021, testIntRead = 0;
	long testLong = 1236532L, testLongRead = 0;
	char testChar = 'a', testCharRead = 0;
	double testDouble = 654.2342, testDoubleRead = 0.0;

	string testString = "test string", testStringRead;

	long offset = 0;
	offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, offset, testInt);
	offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, offset, testLong);
	offset = ProtocolHelper::placeStringIntoByteBuffer(buf, offset, testString);
	offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, offset, testDouble);
	offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, offset, testChar);


	offset = 0;
	offset = ProtocolHelper::extractAtomicDataFromByteBuffer(buf, offset, &testIntRead);
	offset = ProtocolHelper::extractAtomicDataFromByteBuffer(buf, offset, &testLongRead);
	offset = ProtocolHelper::extractStringFromByteBuffer(buf, offset, testStringRead);
	offset = ProtocolHelper::extractAtomicDataFromByteBuffer(buf, offset, &testDoubleRead);
	offset = ProtocolHelper::extractAtomicDataFromByteBuffer(buf, offset, &testCharRead);

	printf("Testing Atomic Data serialization/deserialization:\n");
	{
		printf("\tTest 0 [type: int]:\t"); if(testInt - testIntRead == 0) printf("passed\n"); else printf("not passed; original = %d, read = %d\n", testInt, testIntRead);
		printf("\tTest 1 [type: long]:\t"); if(testLong - testLongRead == 0) printf("passed\n"); else printf("not passed; original = %ld, read = %ld\n", testLong, testLongRead);
		printf("\tTest 2 [type: char]:\t"); if(testChar - testCharRead == 0) printf("passed\n"); else printf("not passed; original = %c, read = %c\n", testChar, testCharRead);
		printf("\tTest 3 [type: double]:\t"); if(fabs(testDouble - testDoubleRead) < 1e-7 ) printf("passed\n"); else printf("not passed; original = %lf, read = %lf\n", testDouble, testDoubleRead);
	}

	printf("Testing String serialization/desrialization:\n");
	printf("\tTest 4 [type: string]:\t"); if(testString == testStringRead) printf("passed\n"); else printf("not passed; original = %s, read = %s\n", testString.c_str(), testStringRead.c_str());

	delete[] buf;
}
void startReceive();

/*void udp_request_handler(boost::array <char, 1024>& buffer, std::size_t bytesReceived)
{
	char *buf = buffer.elems;

	printf("[Thread %u] Received %lu bytes\n", (unsigned int)pthread_self(), bytesReceived);
	for(int i = 0; i < bytesReceived; i++) printf(" %c ", buf[i]);
	putchar('\n');
	startReceive();
}
*/
void* io_service_thread(void* args)
{
	boost::asio::io_service* ioService = (boost::asio::io_service*)args;
	ioService->run();
}

/*void boost_udp_test(unsigned short portNo)
{
	printf("Performing multi threaded boost asio socket test\n");
	pthread_t threadPool[2];
	boost::asio::io_service::work work(ioService);

	pthread_create(&threadPool[0], NULL, io_service_thread, &ioService);
	pthread_create(&threadPool[1], NULL, io_service_thread, &ioService);

	startReceive();

	pthread_join(threadPool[0], NULL);
	pthread_join(threadPool[1], NULL);
}*/

void home_agent_server_test()
{
	printf("Testing home agent server\n");
	HomeAgentServer haServer(11053, 4, 0x03);
}

/*void startReceive()
{
	udpListenSocket.async_receive_from(boost::asio::buffer(buffer), remoteEndpoint, boost::bind(&udp_request_handler, buffer, boost::asio::placeholders::bytes_transferred()));
}*/

void test_cassandradb_driver()
{
	printf("Testing Cassandra Database Driver\n");
	cql::cql_initialize();
	database = boost::dynamic_pointer_cast<CassandraDBDriver>(CassandraDBDriver::getDatabaseDriverObject());
	database->openConnection();

	long nCpuThreads = sysconf(_SC_NPROCESSORS_ONLN);
	printf("# of CPU threads = %ld\n", nCpuThreads);

	pthread_t queryThread[N_THREADS];
	cpu_set_t cpuSet;

	timeval start, end, elapsed;

	pthread_attr_t threadAttr;

	for(int i = 0; i < N_THREADS; i++)
	{
		CPU_ZERO(&cpuSet);
		CPU_SET(i % nCpuThreads, &cpuSet);

		pthread_attr_init(&threadAttr);

		pthread_attr_setaffinity_np(&threadAttr, sizeof(cpu_set_t), &cpuSet);

		printf("Creating Thread %d on %ld CPU\n", i, i % nCpuThreads);
		pthread_create(&queryThread[i], &threadAttr, run_query, &database);
		if(i == 0)
			gettimeofday(&start, NULL);

		CPU_ZERO(&cpuSet);
		//pthread_attr_getaffinity_np(&threadAttr, sizeof(cpu_set_t), &cpuSet);
		//for(int k = 0; k < nCpuThreads; k++) if(CPU_ISSET(k, &cpuSet)) printf("Thread %d assigned to CPU = %d\n", i, k);
	}

	for(int i = 0; i < N_THREADS; i++)
		pthread_join(queryThread[i], NULL);

	gettimeofday(&end, NULL);

	timeval_subtract(&elapsed, &end, &start);
	double elapsedTime = elapsed.tv_sec + ((double)elapsed.tv_usec / 1000000.0);

	printf("[Cassandra Test]\tPerformed %d queries in %.3lf seconds\n", N_THREADS * N_REPS * N_KEYS, elapsedTime);
	//delete database;
	cql::cql_terminate();
}

void *level_db_query(void* args)
{
	leveldb::DB* database = (leveldb::DB*)args;
	string keySet[N_KEYS] = {"key0", "key1", "key3"};
	string value;

	for(int i = 0; i < N_REPS; i++)
		for(int j = 0; j < N_KEYS; j++)
			database->Get(leveldb::ReadOptions(), keySet[j], &value);

}

void level_db_test()
{
	puts("Testing level db");
	const string dbStoragePath = "/home/sr2chowd/UW/pweb_home_agent/storage/leveldb_storage/home_agent.db";

	leveldb::DB* levelDBDatabase;
	leveldb::Options dbOptions;

	dbOptions.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(dbOptions, dbStoragePath, &levelDBDatabase);

	string keySet[N_KEYS] = {"key0", "key1", "key3"};

	for(int i = 0; i < N_KEYS; i++) levelDBDatabase->Put(leveldb::WriteOptions(), keySet[i], keySet[i]);

	long nCpuThreads = sysconf(_SC_NPROCESSORS_ONLN);
	printf("# of CPU threads = %ld\n", nCpuThreads);

	pthread_t queryThread[N_THREADS];
	cpu_set_t cpuSet;

	timeval start, end, elapsed;

	for(int i = 0; i < N_THREADS; i++)
	{
		CPU_ZERO(&cpuSet);
		CPU_SET(i % nCpuThreads, &cpuSet);

		pthread_attr_t* threadAttr = new pthread_attr_t;
		pthread_attr_init(threadAttr);
		pthread_attr_setaffinity_np(threadAttr, sizeof(cpu_set_t), &cpuSet);
		printf("Creating Thread %d on %ld CPU\n", i, i % nCpuThreads);
		pthread_create(&queryThread[i], threadAttr, level_db_query, levelDBDatabase);

		if(i == 0)
			gettimeofday(&start, NULL);

		CPU_ZERO(&cpuSet);
		//pthread_attr_getaffinity_np(&threadAttr, sizeof(cpu_set_t), &cpuSet);
		//for(int k = 0; k < nCpuThreads; k++) if(CPU_ISSET(k, &cpuSet)) printf("Thread %d assigned to CPU = %d\n", i, k);
	}

	for(int i = 0; i < N_THREADS; i++)
		pthread_join(queryThread[i], NULL);

	gettimeofday(&end, NULL);

	timeval_subtract(&elapsed, &end, &start);
	double elapsedTime = elapsed.tv_sec + ((double)elapsed.tv_usec / 1000000.0);

	printf("[LevelDB test]\tPerformed %d queries in %.3lf seconds\n", N_THREADS * N_REPS * N_KEYS, elapsedTime);
	delete levelDBDatabase;
}

int main(int argc, char *argv[])
{
	test_cassandradb_driver();
	//test_protocol_helper();
	//home_agent_server_test();
	//boost_udp_test(1153);
	level_db_test();
	return 0;
}
