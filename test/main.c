/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#include "steam-condenser.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct MasterServer *master;
	struct addrinfo *server;
	sc_init();
	
	master = getMasterServer(SOURCE_MASTER);
	getServers(master, REGION_US_EAST, "\\type\\d\\secure\\1\\linux\\1\\empty\\1\\full\\1\\napp\\500");
	server = master->servers;
	
	while (server != NULL) {
		char *str = malloc(INET_ADDRSTRLEN);
		switch(server->ai_family) {
			case AF_INET:
				inet_ntop(AF_INET, &((struct sockaddr_in*)server->ai_addr)->sin_addr, str, INET_ADDRSTRLEN);
			break;
			case AF_INET6:
				inet_ntop(AF_INET6, &((struct sockaddr_in6*)server->ai_addr)->sin6_addr, str, INET6_ADDRSTRLEN);
			break;
			default:
				strncpy(str, "Unknown AF", INET_ADDRSTRLEN);
		}
		printf("%s\n", str);
		server = server->ai_next;
	}
	
	// do some cleanup...
	freeMasterServer(master);
	sc_end();
	system("pause");
	return 0;
}
