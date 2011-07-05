/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/

#define DEBUG // change to NDEBUG on release

#include "steam-condenser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	sc_MasterServer	*master = NULL;
	sc_ServerList	*server = NULL;
	unsigned int	i = 0;
	sc_init();
	
	master = sc_getMasterServer(SOURCE_MASTER);
	sc_getServers(master, REGION_US_EAST, "\\type\\d\\secure\\1\\linux\\1\\empty\\1\\full\\1\\napp\\500");
	server = master->servers;
	while (server != NULL) {
		printf("%s:%d\n", server->address, server->port);
		server = server->next;
		i++;
	}
	printf("Found %d Servers\n", i);
	
	// do some cleanup...
	sc_freeMasterServer(master);
	sc_end();
	system("pause");
	return 0;
}
