/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#include "MasterServer.h"
#include "steam-condenser.h"
#include <stdio.h>
#include <stdlib.h>

// Connects to, and returns an open socket to an available master server
sc_MasterServer* SC_API(sc_getMasterServer)(const char *address)
{
	sc_MasterServer *master = calloc(sizeof(sc_MasterServer), 1);
	master->socket = sc_openSocketAddr(address, SOCK_DGRAM); // open socket() and connect()
	return master;
}

// Not IPv6 Compatible on account of the master server's response
void SC_API(sc_getServers)(sc_MasterServer *master, sc_Region region, const char *filter)
{
	int i=0, sent=0, recvd=0, addressLen=0;			// bytes sent/recvd, length of game server's address
	unsigned int filterLen = strlen(filter) + 1;	// length of our filter + \0
	sc_ServerList *last = NULL;
	
	char address[ADDRSTRLEN] = "0.0.0.0:0";
	char addr2[ADDRSTRLEN] = "";
	unsigned char buffer[STEAM_PACKET_SIZE] = "";								// buffer for recvd data
	char *message = calloc(sizeof(char), 2 + ADDRSTRLEN + filterLen);	// data to send
	
	do {
		/**
		 * Prepare the packet
		 * Message, Region, IP:Port, Filter
		 */
		addressLen = strlen(address) + 1;
		message[0] = A2M_GET_SERVERS_BATCH2;
		message[1] = region;
		memcpy(&message[2], address, addressLen);
		memcpy(&message[2 + addressLen], filter, filterLen);
		
		if (sent = send(master->socket, message, 2 + addressLen + filterLen, 0) < 2 + addressLen + filterLen) {
			SC_ERRORMSG("Unable to send all data", SC_SEND_ERROR);
			break;
		}
		
		if ((recvd = recv(master->socket, &buffer, 1392, 0)) < 0) {
			SC_ERRORMSG("Error receiving data", SC_RECV_ERROR);
			break;
		}
		
		// Each server listing is 6 bytes. 4*octets, 2*port
		for (i = 0; i < recvd; i += 6) {
			unsigned int addrlen = strlen(inet_ntop(AF_INET, &buffer[i], address, INET_ADDRSTRLEN)); // get next address
			unsigned int port = (buffer[i+4] << 8) | buffer[i+5];
			
			strcpy(addr2, address);
			
			address[addrlen] = ':';
			sprintf(&address[addrlen+1], "%d", port);
			if (strcmp(address, "0.0.0.0:0")) {
				sc_ServerList *list = malloc(sizeof(sc_ServerList));
				strcpy(list->address, addr2);
				sprintf(&list->port, "%d", port);
				list->next = NULL;
				if (last) {
					last->next = list;
				} else {
					master->servers = list;
				}
				last = list;
			}
		}
		//printf("%s\n", address); // last address in packet
	} while (strcmp(address, "0.0.0.0:0"));
	free(message);
}

void SC_API(sc_freeMasterServer)(sc_MasterServer *master)
{
	sc_ServerList *next = NULL;
	while (master->servers != NULL) {
		next = master->servers->next;
		free(master->servers);
		master->servers = next;
	}
	master->servers = NULL;
	sc_closeSocket(&master->socket);
	free(master);
	master = NULL;
}
