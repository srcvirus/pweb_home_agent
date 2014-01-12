#include <stdio.h>
#include "../database/cassandra_db.h"
#include "../interfaces/database_driver.h"
#include "../controllers/home_agent_index_cassandra_controller.h"
#include "../models/home_agent_index.h"
#include <pthread.h>
#include <unistd.h>

#define N_THREADS 9
#define N_KEYS 3
#define N_REPS 1000

timeval start, end, elapsed;

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

int main(int argc, char *argv[])
{
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
	return 0;
}
