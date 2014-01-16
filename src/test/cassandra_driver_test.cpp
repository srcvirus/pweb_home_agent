#include <stdio.h>
#include <boost/asio.hpp>

#include "../database/cassandra_db.h"
#include "../interfaces/database_driver.h"
#include "../controllers/home_agent_index_cassandra_controller.h"
#include "../models/home_agent_index.h"
#include "../protocol/protocol_helper.h"

#include <pthread.h>
#include <unistd.h>

#define N_THREADS 8
#define N_KEYS 3
#define N_REPS 500

timeval start, end, elapsed;

boost::asio::io_service ioService;
boost::asio::ip::udp::endpoint remoteEndpoint;
boost::array <char, 1024> buffer;
boost::asio::ip::udp::socket udpListenSocket(ioService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 1153));

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
	printf("Starting Query Sequence\n");

	CassandraDBDriver* dbDriver = (CassandraDBDriver*)args;
	string key_set[] = {"mypc0", "mypc1", "mypc2"};

	HomeAgentIndexCassandraController* haIndexController = new HomeAgentIndexCassandraController(dbDriver);
	HomeAgentIndex* haIndexObj;

	for(int j = 0; j < N_REPS; j++)
		for(int i = 0; i < N_KEYS; i++)
		{
			haIndexObj = haIndexController->getHomeAgentIndex(key_set[i]);
//			haIndexObj->print();
			delete haIndexObj;
		}

	delete haIndexController;
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

void udp_request_handler(boost::array <char, 1024>& buffer, std::size_t bytesReceived)
{
	char *buf = buffer.elems;

	printf("Received %lu bytes\n", bytesReceived);
	for(int i = 0; i < bytesReceived; i++) printf(" %c ", buf[i]);
	putchar('\n');
	startReceive();
}

void* io_service_thread(void* args)
{
	boost::asio::io_service* ioService = (boost::asio::io_service*)args;
	ioService->run();
}

void boost_udp_test(unsigned short portNo)
{
	printf("Performing multi threaded boost asio socket test\n");
	pthread_t threadPool[2];
	boost::asio::io_service::work work(ioService);

	pthread_create(&threadPool[0], NULL, io_service_thread, &ioService);
	pthread_create(&threadPool[1], NULL, io_service_thread, &ioService);

	startReceive();

	pthread_join(threadPool[0], NULL);
	pthread_join(threadPool[1], NULL);
}

void startReceive()
{
	udpListenSocket.async_receive_from(boost::asio::buffer(buffer), remoteEndpoint, boost::bind(&udp_request_handler, buffer, boost::asio::placeholders::bytes_transferred()));
}

void test_cassandradb_driver()
{
	printf("Testing Cassandra Database Driver\n");
	CassandraDBDriver* database = (CassandraDBDriver*)CassandraDBDriver::getDatabaseDriverObject();
	database->selectKeySpace("pweb");

	long nCpuThreads = sysconf(_SC_NPROCESSORS_ONLN);
	printf("# of CPU threads = %ld\n", nCpuThreads);

	pthread_t queryThread[N_THREADS];
	pthread_attr_t threadAttr;
	cpu_set_t cpuSet;

	pthread_attr_init(&threadAttr);

	timeval start, end, elapsed;

	for(int i = 0; i < N_THREADS; i++)
	{
		CPU_ZERO(&cpuSet);
		CPU_SET(i % nCpuThreads, &cpuSet);
		pthread_attr_setaffinity_np(&threadAttr, sizeof(cpu_set_t), &cpuSet);

		printf("Creating Thread %d\n", i);
		pthread_create(&queryThread[i], &threadAttr, run_query, database);
		if(i == 0)
			gettimeofday(&start, NULL);

		CPU_ZERO(&cpuSet);
		pthread_attr_getaffinity_np(&threadAttr, sizeof(cpu_set_t), &cpuSet);
		for(int k = 0; k < nCpuThreads; k++) if(CPU_ISSET(k, &cpuSet)) printf("Thread %d assigned to CPU = %d\n", i, k);
	}

	for(int i = 0; i < N_THREADS; i++)
		pthread_join(queryThread[i], NULL);

	gettimeofday(&end, NULL);

	timeval_subtract(&elapsed, &end, &start);
	double elapsedTime = elapsed.tv_sec + ((double)elapsed.tv_usec / 1000000.0);

	printf("Performed %d queries in %ld:%ld [%.3lf seconds]\n", N_THREADS * N_REPS * N_KEYS, elapsed.tv_sec, elapsed.tv_usec, elapsedTime);

	delete database;
}
int main(int argc, char *argv[])
{
	test_cassandradb_driver();
	//test_protocol_helper();
	//boost_udp_test(1053);
	return 0;
}
