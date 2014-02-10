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
#define DEFAULT_HTTP_LISTEN_PORT 80
#define DEFAULT_HOST_NAME	"localhost"
#define DEFAULT_SUFFIX	"dht.pwebproject.net"
#define USAGE_STRING	"Usage: pweb_home_agent [-a <home_agent_alias> | --alias=<home_agent_alias>] [optional args]\nOptional Args"

#endif /* GLOBAL_H_ */
