/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG // change to NDEBUG on release

#include "steam-condenser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printServers();
void printServerInfo(const char *address);

int main(int argc, char *argv[])
{
	char addr[ADDRSTRLEN] = "77.220.184.197:27356";
	
	sc_init();
	
	printServers();
	system("pause");
	
	printServerInfo(addr);
	system("pause");
	
	sc_end();
	return 0;
}

void printServerInfo(const char *address)
{
	sc_GameServer *server = NULL;
	server = sc_getGameServerFromString(address);
	sc_getPing(server);
	//sc_getServerInfo(server);
	printf("Ping: %d\n", server->ping);
}

void printServers()
{
	sc_MasterServer	*master = NULL;
	sc_ServerList	*server = NULL;
	unsigned int	i = 0;
	
	master = sc_getMasterServer(SOURCE_MASTER);
	sc_getServers(master, REGION_US_EAST, "\\type\\d\\secure\\1\\linux\\1\\empty\\1\\full\\1\\napp\\500");
	server = master->servers;
	while (server != NULL) {
		printf("%s:%s\n", server->address, server->port);
		server = server->next;
		i++;
	}
	printf("Found %d Servers\n", i);
	
	// do some cleanup...
	sc_freeMasterServer(master);
}

#ifdef __cplusplus
}
#endif
