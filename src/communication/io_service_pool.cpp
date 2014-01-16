/*
 * io_service_pool.cpp
 *
 *  Created on: Jan 16, 2014
 *      Author: sr2chowd
 */

#include "io_service_pool.h"
#include <stdio.h>
#include <sched.h>
#include <algorithm>
#include <vector>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace std;


const size_t IOServicePool::MAX_CPU_IO_THREADS = sysconf(_SC_NPROCESSORS_ONLN);

IOServicePool::IOServicePool(unsigned int poolSize, unsigned long cpuPinnedServiceMask)
{
	int i;
	this->nIOServicePoolSize = poolSize;
	this->nextCPUPinnedIOServiceIndex = -1;
	this->nextNonPinnedIOServiceIndex = -1;
	this->nextIOServiceIndex = -1;

	for(i = 0; i < poolSize; ++i)
	{
		this->ioServicePool.push_back(new boost::asio::io_service());
		this->ioServiceWorkPool.push_back(new boost::asio::io_service::work(*this->ioServicePool[i]));

		if((cpuPinnedServiceMask & 1) && cpuPinnedServices.size() < IOServicePool::MAX_CPU_IO_THREADS)
			cpuPinnedServices.push_back(i);
		else nonPinnedServices.push_back(i);
		cpuPinnedServiceMask >>= 1;
	}
}

void IOServicePool::startServices()
{
	std::vector <pthread_t> threadPool;
	int i;
	int nextPinnedCPU = -1;

	for(i = 0; i < nIOServicePoolSize; ++i)
	{
		pthread_t nextThread;
		pthread_attr_t* threadAttribute = NULL;

		if(binary_search(cpuPinnedServices.begin(), cpuPinnedServices.end(), i))
		{
			threadAttribute = new pthread_attr_t;
			cpu_set_t cpuSet;
			pthread_attr_init(threadAttribute);
			CPU_ZERO(&cpuSet);
			CPU_SET(++nextPinnedCPU, &cpuSet);
			pthread_attr_setaffinity_np(threadAttribute, sizeof(cpu_set_t), &cpuSet);
		}

		pthread_create(&nextThread, threadAttribute, &IOServicePool::ioServiceThreadMethod, &this->ioServicePool[i]);
		threadPool.push_back(nextThread);
	}

	for(i = 0; i < nIOServicePoolSize; ++i)
		pthread_join(threadPool[i], NULL);
}

void* IOServicePool::ioServiceThreadMethod(void* args)
{
	boost::asio::io_service* ioService = *((boost::asio::io_service**)args);
	printf("Starting i/o thread 0x%x, with io_service pointer %p\n", (unsigned int)pthread_self(), ioService);
	ioService->run();
}

boost::asio::io_service& IOServicePool::getNonPinnedIOService()
{
	//return a io_service based on round robin scheduling
	nextNonPinnedIOServiceIndex++;
	nextNonPinnedIOServiceIndex %= nonPinnedServices.size();
	return *this->ioServicePool[this->nonPinnedServices[nextNonPinnedIOServiceIndex]];
}

boost::asio::io_service& IOServicePool::getPinnedIOService()
{
	nextCPUPinnedIOServiceIndex++;
	nextCPUPinnedIOServiceIndex %= cpuPinnedServices.size();
	return *this->ioServicePool[this->cpuPinnedServices[nextCPUPinnedIOServiceIndex]];
}

boost::asio::io_service& IOServicePool::getIOService()
{
	nextIOServiceIndex++;
	nextIOServiceIndex %= nIOServicePoolSize;
	return *this->ioServicePool[nextIOServiceIndex];
}

