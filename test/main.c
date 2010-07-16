/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#include "../include/steam-condenser.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int masterSocket;
	struct addrinfo *servers;
	sc_init();
	
	masterSocket = getMasterServer(SOURCE_MASTER);
	servers = getServers(masterSocket, REGION_US_EAST, "\\type\\d\\secure\\1\\linux\\1\\empty\\1\\full\\1\\napp\\500");
	
	while (servers != NULL) {
		char *str = malloc(INET_ADDRSTRLEN);
		switch(servers->ai_family) {
			case AF_INET:
				inet_ntop(AF_INET, &((struct sockaddr_in*)servers->ai_addr)->sin_addr, str, INET_ADDRSTRLEN);
			break;
			case AF_INET6:
				inet_ntop(AF_INET6, &((struct sockaddr_in6*)servers->ai_addr)->sin6_addr, str, INET6_ADDRSTRLEN);
			break;
			default:
				strncpy(str, "Unknown AF", INET_ADDRSTRLEN);
		}
		printf("%s\n", str);
		servers = servers->ai_next;
	}
	
	// do some cleanup...
	freeaddrinfo(servers);
	#ifdef WIN32
		closesocket(masterSocket);
	#else
		close(masterSocket);
	#endif
	sc_end();
	system("pause");
	return 0;
}
