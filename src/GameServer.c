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

sc_GameServer* SC_API(sc_getGameServerFromString)(const char *address)
{
	sc_GameServer *ret = calloc(sizeof(sc_GameServer), 1);
	ret->socketUDP = sc_openSocketAddr(address, SOCK_DGRAM); // open socket() and connect()
	return ret;
}

sc_GameServer* SC_API(sc_getGameServer)(const char *address, const char *port)
{
	sc_GameServer *ret = calloc(sizeof(sc_GameServer), 1);
	ret->socketUDP = sc_openSocketAddrPort(address, port, SOCK_DGRAM); // open socket() and connect()
	return ret;
}

int SC_API(sc_getPing)(sc_GameServer *server)
{
	clock_t starttime = clock() * CLOCKS_PER_SEC;
	int sent, recvd;
	char buffer[STEAM_PACKET_SIZE] = "";
	sent = recvd = 0;
	
	if (sent = send(server->socketUDP, A2A_PING, A2A_PING_SIZE, 0) < A2A_PING_SIZE) {
		fprintf(stderr, "Unable to send all data");
	}
	
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	if (buffer[4] == 0x6A) { // 'j'
		clock_t endtime = clock() * CLOCKS_PER_SEC;
		server->ping = (endtime - starttime) / 1000;
	} else {
		server->ping = -1;
	}
	return server->ping;
}

void SC_API(sc_getServerInfo)(sc_GameServer *server)
{
	BOOL done = FALSE;
	BOOL isCompressed = FALSE;
	byte packetNum = 0;
	byte packetTotal = 0;
	short splitLength = 0;
	int sent, recvd;
	unsigned int pos = 0;
	unsigned int finalSize = 0;
	unsigned int crc32 = 0;
	unsigned int requestID = 0;
	unsigned char buffer[STEAM_PACKET_SIZE] = "";
	sent = recvd = packetNum = packetTotal = 0;
	
	if (sent = send(server->socketUDP, A2S_INFO, A2S_INFO_SIZE, 0) < 0) {
		fprintf(stderr, "Unable to send all data");
	}
	
	// split this into another func or two
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	
	if (sc_readLong(&buffer, &pos) == -2) {
		byte numPackets = 1;
		do {
			requestID		= sc_readLong(&buffer, &pos);
			isCompressed	= ((requestID & 0x80000000) != 0);
			packetNum		= sc_readByte(&buffer, &pos) >> 4;
			pos--;
			packetTotal		= sc_readByte(&buffer, &pos) & 0xF;
			// try to assemble all the packets
			
			// get the next packet
			pos = 0;
			numPackets++;
			recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
			if (recvd == -1 && errno == EAGAIN) {
				done = TRUE;
				fprintf(stderr, "Timeout receiving");
			}
		} while (!done && numPackets < packetTotal && sc_readLong(&buffer, &pos) == -2);
	} else {
		// not split or compressed
	}
}

void SC_API(sc_getChallenge)(sc_GameServer *server)
{
	
}

void SC_API(sc_getPlayers)(sc_GameServer *server)
{
	
}

void SC_API(sc_getRules)(sc_GameServer *server)
{
	
}

void SC_API(sc_updatePlayers)(sc_GameServer *server)
{
	
}

void SC_API(sc_updateRules)(sc_GameServer *server)
{
	
}


void SC_API(sc_freePlayers)(sc_Players *players)
{
	
}

void SC_API(sc_freeRules)(sc_Rules *players)
{
	
}

void SC_API(sc_freeGameServer)(sc_GameServer *server)
{
	
}
