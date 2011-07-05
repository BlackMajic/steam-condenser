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
#include <stdlib.h>
#include <stdio.h>

sc_GameServer* SC_API(sc_getGameServerFromString)(char *address)
{
	sc_GameServer *ret = calloc(sizeof(sc_GameServer), 1);
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

sc_GameServer* SC_API(sc_getGameServerFromAddress)(struct addrinfo *address)
{
	sc_GameServer *ret = calloc(sizeof(sc_GameServer), 1);
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

void SC_API(getPing)(sc_GameServer *server)
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

void SC_API(getServerInfo)(sc_GameServer *server)
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
	
	if (sc_readLong(buffer, &pos) == -2) { // split packet
		do {
		} while (0);
	}
}

void SC_API(getChallenge)(sc_GameServer *server)
{
	
}

void SC_API(getPlayers)(sc_GameServer *server)
{
	
}

void SC_API(getRules)(sc_GameServer *server)
{
	
}

void SC_API(updatePlayers)(sc_GameServer *server)
{
	
}

void SC_API(updateRules)(sc_GameServer *server)
{
	
}


void SC_API(freePlayers)(sc_Players *players)
{
	
}

void SC_API(freeRules)(sc_Rules *players)
{
	
}

void SC_API(freeGameServer)(sc_GameServer *server)
{
	
}
