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
extern struct MasterServer* getMasterServer(const char *address)
{
	struct MasterServer *master = calloc(sizeof(struct MasterServer), 1);
	struct addrinfo hints, *servers, *server;
	char *serv = strdup(address);
	char *addr = strtok(serv, ":");
	char *port = strtok(NULL, ":");
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	if (getaddrinfo(addr, port, &hints, &servers) != 0) {
		perror("getaddrinfo");
		exit(2);
	}
	
	for (server = servers; server != NULL; server = server->ai_next) {
		master->socket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
		if (master->socket == -1) {
			perror("Sockout Error");
		} else {
			if (connect(master->socket, server->ai_addr, server->ai_addrlen) == -1) {
				perror("Connect Error");
			} else {
				break;
			}
		}
	}
	if (master->socket == -1) exit(3);
	freeaddrinfo(servers);
	free(serv);
	
	return master;
}

// IPv6 Incompatible
extern void getServers(struct MasterServer *master, const char region, const char *filter)
{
	int i, sent, recvd, addressLen;
	int filterLen = strlen(filter) + 1;
	struct addrinfo *last, *my;
	struct addrinfo *ret = calloc(sizeof(struct addrinfo), 1);
	
	char address[INET_ADDRSTRLEN] = "0.0.0.0:0";
	char *buffer = calloc(sizeof(char), 1392);
	char *message = calloc(sizeof(char), 2 + INET_ADDRSTRLEN + filterLen);
	last = ret;
	do {
		addressLen = strlen(address) + 1;
		message[0] = A2M_GET_SERVERS_BATCH2;
		message[1] = region;
		memcpy(&message[2], address, addressLen);
		memcpy(&message[2 + addressLen], filter, filterLen);
		if (sent = send(master->socket, message, 2 + addressLen + filterLen, 0) < 2 + addressLen + filterLen) {
			fprintf(stderr, "Unable to send all data");
		}
		
		recvd = recv(master->socket, buffer, 1392, 0);
		
		for (i = 0; i < recvd; i += 6) {
			int len, port;
			unsigned char j, k;
			char *serv, *addr, *cPort;
			struct addrinfo hints;
			len = strlen(inet_ntop(AF_INET, &buffer[i], address, INET_ADDRSTRLEN));
			j = buffer[i+4];
			k = buffer[i+5];
			address[len] = ':';
			port = (j << 8) | k;
			sprintf(&address[len+1], "%d", port);
			
			serv = strdup(address);
			addr = strtok(serv, ":");
			cPort = strtok(NULL, ":");
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_flags = AI_PASSIVE;
			if (getaddrinfo(addr, cPort, &hints, &my) != 0) {
				perror("getaddrinfo");
				exit(5);
			}
			
			if (ret->ai_addr == 0) {
				memcpy(ret, my, sizeof(struct addrinfo));
			} else {
				last->ai_next = calloc(sizeof(struct addrinfo), 1);
				memcpy(last->ai_next, my, sizeof(struct addrinfo));
				memcpy(last->ai_addr, my->ai_addr, sizeof(struct sockaddr));
				last = last->ai_next;
			}
		}
		//printf("%s\n", address); // last address in packet
	} while (strcmp(address, "0.0.0.0:0"));
	free(buffer);
	free(message);
	
	master->servers = ret;
}

extern void freeMasterServer(struct MasterServer *master)
{
	struct addrinfo *server;
	//freeaddrinfo(master->servers);
	server = master->servers;
	while (server != NULL) {
		struct addrinfo *s = server->ai_next;
		free(server->ai_addr);
		free(server);
		server = s;
	}
	master->servers = NULL;
	shutdown(master->socket, 2);
	master->socket = NULL;
	free(master);
}
