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
#include <errno.h>
#include "bzlib.h"

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

// depricated - use A2S_INFO instead
int SC_API(sc_getPing)(sc_GameServer *server)
{
	clock_t starttime = clock() * CLOCKS_PER_SEC;
	int sent, recvd;
	unsigned char buffer[STEAM_PACKET_SIZE] = "";
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

char* SC_API(sc_combineSplitPackets)(int socket, char *buffer, int *pos, BOOL isGoldSrc)
{
	unsigned long requestID = 0;
	unsigned long checksum = 0;
	unsigned long fullDataSize = 0;
	unsigned short splitSize = 0;
	unsigned char *packet = NULL;
	int recvd = 0;
	int sent = 0;
	byte packetCount = 0;
	byte packetNumber = 0;
	byte numPackets = 1;
	BOOL isCompressed = FALSE;
	
	do {
		if (requestID) {
			if (requestID != sc_readLong(buffer, pos)) {
				// ignore the packet
				numPackets--;
				recvd = 0;
				pos = 0;
			}
		} else {
			requestID = sc_readLong(buffer, pos);
		}
		if ((*pos) > 0) {
			if (!isGoldSrc) {
				isCompressed	= ((requestID & 0x80000000) != 0);
				packetNumber	= sc_readByte(buffer, pos);
				packetCount		= sc_readByte(buffer, pos);
				splitSize		= sc_readShort(buffer, pos);	// expect 0x04E0
				
				if (isCompressed) {
					fullDataSize	= sc_readLong(buffer, pos);
					checksum		= sc_readLong(buffer, pos);
					if (!packet) packet = calloc(sizeof(char), fullDataSize);
				}
			} else {
				packetNumber	= sc_readByte(buffer, pos);
				packetCount		= packetNumber & 0xF;
				packetNumber	= packetNumber >> 4;
			}
			
			// populate packet
		}
		
		if (numPackets < packetCount) {
			recvd = recv(socket, buffer, STEAM_PACKET_SIZE, 0);
			if (recvd == -1) {
				if (errno == EAGAIN)
					fprintf(stderr, "Timed out");
				else
					fprintf(stderr, "Couldn't receive data");
			} else {
				numPackets++;
				(*pos) = 0;
			}
		} else {
			recvd = 0;
			(*pos) = 0;
		}
		
	} while (recvd > 0 && sc_readLong(buffer, pos) == -2);
	return packet;
}

void SC_API(sc_getServerInfo)(sc_GameServer *server, BOOL isGoldSrc)
{
	int sent = 0, recvd = 0;
	unsigned int pos = 0;
	unsigned char buffer[STEAM_PACKET_SIZE] = "";
	unsigned char *packet = NULL;
	clock_t starttime = clock() * CLOCKS_PER_SEC;
	
	if (sent = send(server->socketUDP, A2S_INFO, A2S_INFO_SIZE, 0) < 0) {
		fprintf(stderr, "Unable to send all data");
	}
	
	// split this into another func or two
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	server->ping = ((clock() * CLOCKS_PER_SEC) - starttime) / 1000;
	if (sc_readLong(&buffer, &pos) == -2) {
		packet = sc_combineSplitPackets(server->socketUDP, &buffer, &pos, isGoldSrc);
	}
	if (!packet) packet = &buffer;
	
	server->info.type				= sc_readByte(packet, &pos);
	if (!isGoldSrc) {
		server->info.npVersion		= sc_readByte(packet, &pos);
		sc_readString(&server->info.hostname, packet, &pos);
		sc_readString(&server->info.map, packet, &pos);
		sc_readString(&server->info.gameDir, packet, &pos);
		sc_readString(&server->info.gameDesc, packet, &pos);
		server->info.appID			= sc_readShort(packet, &pos);
		server->numPlayers			= sc_readByte(packet, &pos);
		server->info.maxPlayers		= sc_readByte(packet, &pos);
		server->info.numBots		= sc_readByte(packet, &pos);
		server->info.dedicated		= sc_readByte(packet, &pos);
		server->info.os				= sc_readByte(packet, &pos);
		server->info.password		= sc_readByte(packet, &pos);
		server->info.secure			= sc_readByte(packet, &pos);
		sc_readString(&server->info.gameVersion, packet, &pos);
		server->info.EDF			= sc_readByte(packet, &pos);
		if (server->info.EDF & 0x80)
			server->info.port		= sc_readShort(packet, &pos);
		if (server->info.EDF & 0x10)
			server->info.steamID	= sc_readLongLong(packet, &pos);
		if (server->info.EDF & 0x40) {
			server->info.specPort	= sc_readShort(packet, &pos);
			sc_readString(&server->info.specName, packet, &pos);
		}
		if (server->info.EDF & 0x20)
			sc_readString(&server->info.tags, packet, &pos);
		if (server->info.EDF & 0x01)
			server->info.appID2		= sc_readShort(packet, &pos);
	} else {
		sc_readString(&server->info.gameIP, packet, &pos);
		sc_readString(&server->info.hostname, packet, &pos);
		sc_readString(&server->info.map, packet, &pos);
		sc_readString(&server->info.gameDir, packet, &pos);
		sc_readString(&server->info.gameDesc, packet, &pos);
		server->info.numPlayers		= sc_readByte(packet, &pos);
		server->info.maxPlayers		= sc_readByte(packet, &pos);
		server->info.npVersion		= sc_readByte(packet, &pos);
		server->info.dedicated		= sc_readByte(packet, &pos);
		server->info.os				= sc_readByte(packet, &pos);
		server->info.password		= sc_readByte(packet, &pos);
		server->info.isMod			= sc_readByte(packet, &pos);
		if (server->info.isMod) {
			sc_readString(&server->info.modInfo.urlInfo, packet, &pos);
			sc_readString(&server->info.modInfo.urlDL, packet, &pos);
			sc_readByte(packet, &pos);
			server->info.modInfo.version	= sc_readLong(packet, &pos);
			server->info.modInfo.dlSize		= sc_readLong(packet, &pos);
			server->info.modInfo.svOnly		= sc_readByte(packet, &pos);
			server->info.modInfo.clDLL		= sc_readByte(packet, &pos);
		}
		server->info.secure			= sc_readByte(packet, &pos);
		server->info.numBots		= sc_readByte(packet, &pos);
	}
	if (packet != &buffer) free(packet);
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
