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

typedef enum {
	REGION_US_EAST,
	REGION_US_WEST,
	REGION_SOUTH_AMERICA,
	REGION_EUROPE,
	REGION_ASIA,
	REGION_AUSTRALIA,
	REGION_MIDDLE_EAST,
	REGION_AFRICA,
	REGION_ALL				= 0xFF
} sc_Region;

#define A2M_GET_SERVERS_BATCH2	0x31
#define M2A_SERVER_BATCH_HEADER	0x66

typedef struct sc_ServerList {
	char address[ADDRSTRLEN];
	char port[6];
	struct sc_ServerList *next;
} sc_ServerList;

typedef struct {
	int socket;
	sc_ServerList *servers;
} sc_MasterServer;

SC_EXTERN sc_MasterServer*	SC_API(sc_getMasterServer)	(const char *address);
SC_EXTERN void				SC_API(sc_getServers)		(sc_MasterServer *master, sc_Region region, const char *filter);
SC_EXTERN void				SC_API(sc_freeMasterServer)	(sc_MasterServer *master);

#ifdef __cplusplus
}
#endif

#endif
