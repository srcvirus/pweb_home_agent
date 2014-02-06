/*
 * global.h
 *
 *  Created on: Feb 2, 2014
 *      Author: sr2chowd
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <unistd.h>

#define MAX_UDP_BUFFER_SIZE 65536
#define DNS_HEADER_LENGTH	12

#define N_CPU_THREADS	(sysconf(_SC_NPROCESSORS_ONLN))
#define DEFAULT_LISTEN_PORT	53
#define DEFAULT_HOST_NAME	"localhost"

#endif /* GLOBAL_H_ */
