/*
 * io_service_pool.h
 *
 *  Created on: Jan 16, 2014
 *      Author: sr2chowd
 */

#ifndef IO_SERVICE_POOL_H_
#define IO_SERVICE_POOL_H_

#include <vector>
#include <boost/asio.hpp>

using namespace std;

class IOServicePool {
  size_t nIOServicePoolSize; // This is also equal to the number of IOService
                             // threads
  int nextCPUPinnedIOServiceIndex;
  int nextNonPinnedIOServiceIndex;
  int nextIOServiceIndex;
  unsigned long dedicatedServices;

  vector<boost::asio::io_service *> ioServicePool;
  vector<boost::asio::io_service::work *> ioServiceWorkPool;

  vector<int> cpuPinnedServices;
  vector<int> nonPinnedServices;

  static void *ioServiceThreadMethod(void *args);

public:

  IOServicePool(unsigned int poolSize,
                unsigned long cpuPinnedServiceMask = 0x01);
  boost::asio::io_service &getNonPinnedIOService();
  boost::asio::io_service &getPinnedIOService();
  boost::asio::io_service &getIOService();
  boost::asio::io_service &getDedicatedIOService();

  void startServices();

  static const size_t MAX_CPU_IO_THREADS;
};

#endif /* IO_SERVICE_POOL_H_ */
