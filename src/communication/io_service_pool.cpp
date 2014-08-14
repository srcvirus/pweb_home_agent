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

IOServicePool::IOServicePool(unsigned int poolSize,
                             unsigned long cpuPinnedServiceMask) {
  int i;
  this->nIOServicePoolSize = poolSize;
  this->nextCPUPinnedIOServiceIndex = -1;
  this->nextNonPinnedIOServiceIndex = -1;
  this->nextIOServiceIndex = -1;
  this->dedicatedServices = 0x0;

  for (i = 0; i < poolSize; ++i) {
    this->ioServicePool.push_back(new boost::asio::io_service());
    this->ioServiceWorkPool
        .push_back(new boost::asio::io_service::work(*this->ioServicePool[i]));

    if ((cpuPinnedServiceMask & 1) &&
        cpuPinnedServices.size() < IOServicePool::MAX_CPU_IO_THREADS)
      cpuPinnedServices.push_back(i);
    else
      nonPinnedServices.push_back(i);
    cpuPinnedServiceMask >>= 1;
  }
}

void IOServicePool::startServices() {
  std::vector<pthread_t> threadPool;
  int i;
  int nextPinnedCPU = -1;

  for (i = 0; i < nIOServicePoolSize; ++i) {
    pthread_t nextThread;
    pthread_attr_t *threadAttribute = NULL;

    if (binary_search(cpuPinnedServices.begin(), cpuPinnedServices.end(), i)) {
      threadAttribute = new pthread_attr_t;
      cpu_set_t cpuSet;
      pthread_attr_init(threadAttribute);
      CPU_ZERO(&cpuSet);
      CPU_SET(++nextPinnedCPU, &cpuSet);
      pthread_attr_setaffinity_np(threadAttribute, sizeof(cpu_set_t), &cpuSet);
    }

    pthread_create(&nextThread, threadAttribute,
                   &IOServicePool::ioServiceThreadMethod,
                   &this->ioServicePool[i]);
    threadPool.push_back(nextThread);
  }

  for (i = 0; i < nIOServicePoolSize; ++i)
    pthread_join(threadPool[i], NULL);

  printf("All threads joined. Exiting..!\n");
}

void *IOServicePool::ioServiceThreadMethod(void *args) {
  boost::asio::io_service *ioService = *((boost::asio::io_service **)args);
  printf("Starting i/o thread 0x%lx, with io_service pointer %p\n",
         pthread_self(), ioService);
  ioService->run();
}

boost::asio::io_service &IOServicePool::getNonPinnedIOService() {
  // return a io_service based on round robin scheduling
  while (true) {
    nextNonPinnedIOServiceIndex =
        (nextNonPinnedIOServiceIndex + 1) % nonPinnedServices.size();
    if (!(dedicatedServices & (1 << nextNonPinnedIOServiceIndex)))
      break;
  }
  return *this
      ->ioServicePool[this->nonPinnedServices[nextNonPinnedIOServiceIndex]];
}

boost::asio::io_service &IOServicePool::getPinnedIOService() {
  while (true) {
    nextCPUPinnedIOServiceIndex =
        (nextCPUPinnedIOServiceIndex + 1) % cpuPinnedServices.size();
    if (!(dedicatedServices & (1 << nextCPUPinnedIOServiceIndex)))
      break;
  }
  return *this
      ->ioServicePool[this->cpuPinnedServices[nextCPUPinnedIOServiceIndex]];
}

boost::asio::io_service &IOServicePool::getIOService() {
  while (true) {
    nextIOServiceIndex = (nextIOServiceIndex + 1) % nIOServicePoolSize;
    if (!(dedicatedServices & (1 << nextIOServiceIndex)))
      break;
  }
  return *this->ioServicePool[nextIOServiceIndex];
}

boost::asio::io_service &IOServicePool::getDedicatedIOService() {
  nextCPUPinnedIOServiceIndex =
      (nextCPUPinnedIOServiceIndex + 1) % cpuPinnedServices.size();
  dedicatedServices |= (1 << nextCPUPinnedIOServiceIndex);
  return *this->ioServicePool[nextCPUPinnedIOServiceIndex];
}
