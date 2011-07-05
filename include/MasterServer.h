/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#ifndef MASTERSERVER_H
#define MASTERSERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "steam-condenser.h"

#define GOLDSRC_MASTER	"hl1master.steampowered.com:27010"
#define SOURCE_MASTER	"hl2master.steampowered.com:27011"
#define RDKF_MASTER		"67.132.200.140:27012"
#define SIN1_MASTER		"69.28.151.162:27010"

#define REGION_US_EAST			0x00
#define REGION_US_WEST			0x01
#define REGION_SOUTH_AMERICA	0x02
#define REGION_EUROPE			0x03
#define REGION_ASIA				0x04
#define REGION_AUSTRALIA		0x05
#define REGION_MIDDLE_EAST		0x06
#define REGION_AFRICA			0x07
#define REGION_ALL				0xFF

#define A2M_GET_SERVERS_BATCH2	0x31
#define M2A_SERVER_BATCH_HEADER	0x66

typedef struct sc_ServerList {
	char address[ADDRSTRLEN];
	unsigned int port;
	struct sc_ServerList *next;
} sc_ServerList;

typedef struct {
	int socket;
	sc_ServerList *servers;
} sc_MasterServer;

SC_EXTERN sc_MasterServer*	SC_API(sc_getMasterServer)	(const char *address);
SC_EXTERN void				SC_API(sc_getServers)		(sc_MasterServer *master, const byte region, const char *filter);
SC_EXTERN void				SC_API(sc_freeMasterServer)	(sc_MasterServer *master);

#ifdef __cplusplus
}
#endif

#endif
