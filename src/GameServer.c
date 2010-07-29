/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#include "GameServer.h"
#include "steam-condenser.h"

#include <time.h>

extern struct GameServer* getGameServerFromString(char *address)
{
	struct GameServer *ret = calloc(sizeof(struct GameServer), 1);
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
		ret->socketUDP = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
		if (ret->socketUDP == -1) {
			perror("Sockout Error");
		} else {
			if (connect(ret->socketUDP, server->ai_addr, server->ai_addrlen) == -1) {
				perror("Connect Error");
			} else {
				memcpy(ret->addr, server, sizeof(struct addrinfo));
				memcpy(ret->addr->ai_addr, server->ai_addr, sizeof(struct sockaddr));
				break;
			}
		}
	}
	if (ret->socketUDP == -1) exit(3);
	freeaddrinfo(servers);
	free(serv);
	
	return ret;
}

extern struct GameServer* getGameServerFromAddress(struct addrinfo *address)
{
	struct GameServer *ret = calloc(sizeof(struct GameServer), 1);
	struct addrinfo *server;
	for (server = address; server != NULL; server = server->ai_next) {
		ret->socketUDP = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
		if (ret->socketUDP == -1) {
			perror("Sockout Error");
		} else {
			if (connect(ret->socketUDP, server->ai_addr, server->ai_addrlen) == -1) {
				perror("Connect Error");
			} else {
				memcpy(ret->addr, server, sizeof(struct addrinfo));
				memcpy(ret->addr->ai_addr, server->ai_addr, sizeof(struct sockaddr));
				break;
			}
		}
	}
	if (ret->socketUDP == -1) exit(3);
	
	return ret;
}

extern void getPing(struct GameServer *server)
{
	clock_t starttime = clock() * CLOCKS_PER_SEC;
	int sent, recvd;
	char buffer[1400];
	sent = recvd = 0;
	if (sent = send(server->socketUDP, A2A_PING, strlen(A2A_PING), 0)) {
		fprintf(stderr, "Unable to send all data");
	}
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	if (buffer[0] == '0x6A') { // 'j'
		clock_t endtime = clock() * CLOCKS_PER_SEC;
		server->ping = endtime-starttime;
		//return server->ping;
	}
	server->ping = -1;
	//return -1;
}

extern void getServerInfo(struct GameServer *server)
{
	//A2A_INFO;
	//char *message = malloc(size);
	BOOL isCompressed = FALSE;
	int sent, recvd, pos;
	char *buffer = calloc(STEAM_PACKET_SIZE, 1);
	if (sent = send(server->socketUDP, A2S_INFO, strlen(A2S_INFO)+1, 0)) {
		fprintf(stderr, "Unable to send all data");
	}
	
	recvd = recv(server->socketUDP, buffer, STEAM_PACKET_SIZE, 0);
	
	if (readLong(buffer, &pos) == -2) { // split packet
		do {
		} while (0);
	}
}

extern void getChallenge(struct GameServer *server)
{
	
}

extern void getPlayers(struct GameServer *server)
{
	
}

extern void getRules(struct GameServer *server)
{
	
}

extern void updatePlayers(struct GameServer *server)
{
	
}

extern void updateRules(struct GameServer *server)
{
	
}


extern void freePlayers(struct Players *players)
{
	
}

extern void freeRules(struct Rules *players)
{
	
}

extern void freeGameServer(struct GameServer *server)
{
	
}
