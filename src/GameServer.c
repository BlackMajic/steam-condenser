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
	if (buffer[4] == A2A_PING_ACK) { // 'j'
		clock_t endtime = clock() * CLOCKS_PER_SEC;
		server->ping = (endtime - starttime) / 1000;
	} else {
		server->ping = -1;
	}
	return server->ping;
}

char* SC_API(sc_combineSplitPackets)(int socket, char *buffer, int *pos, int recvd, BOOL isGoldSrc)
{
	unsigned long requestID = 0;
	unsigned long checksum = 0;
	unsigned long fullDataSize = 0;
	unsigned short splitSize = 0;
	unsigned char *packet = NULL;
	//int recvd = 0;
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
				packetCount		= sc_readByte(buffer, pos);
				packetNumber	= sc_readByte(buffer, pos);
				splitSize		= 0x04E0;
				
				if (!isCompressed) {
					splitSize		= sc_readShort(buffer, pos);	// expect 0x04E0
				} else {
					fullDataSize	= sc_readLong(buffer, pos);
					checksum		= sc_readLong(buffer, pos);
				}
			} else {
				packetNumber	= sc_readByte(buffer, pos);
				packetCount		= packetNumber & 0xF;
				packetNumber	= packetNumber >> 4;
				splitSize		= 0x04E0;
			}
			
			// populate packet
			if (!packet) {
				packet = calloc(splitSize * packetCount, 1);
			}
			// TODO: Fix packet fragmentation while re-packing
			memcpy(&packet[splitSize * packetNumber], &buffer[(*pos)], splitSize);
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
	if (isCompressed) {
		// decompress
		int bz = 0;
		unsigned char *temp = malloc(fullDataSize);
		if (bz = BZ2_bzBuffToBuffDecompress(temp, &fullDataSize,
											packet, splitSize * packetCount,
											0, 0) == BZ_OK)
		{
			// TODO: crc32 check
		} else {
			fprintf(stderr, "BZ Decompress failed, returned code %d", bz);
		}
		free(packet);
		packet = temp;
	}
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
	
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	server->ping = ((clock() * CLOCKS_PER_SEC) - starttime) / 1000;
	if (sc_readLong(&buffer, &pos) == -2) {
		packet = sc_combineSplitPackets(server->socketUDP, &buffer, &pos, recvd, isGoldSrc);
		if (sc_readLong(packet, &pos) != -1) return;
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
	int sent = 0, recvd = 0;
	unsigned int pos = 0;
	unsigned char buffer[STEAM_PACKET_SIZE] = "";
	char message[10] = "\xFF\xFF\xFF\xFF\x55\0xFF\0xFF\0xFF\0xFF";
	// get a challenge number
	if (sent = send(server->socketUDP, &message, A2S_RULES_SIZE+4, 0) < 0) {
		fprintf(stderr, "Unable to send all data");
	}
	
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	if (recvd == -1) {
		fprintf(stderr, "error receiving data");
	}
	
	sc_readLong(&buffer, &pos); // -1
	if (sc_readByte(&buffer, &pos) == A2S_SERVERQUERY_GETCHALLENGE_ACK) {
		server->challenge = sc_readLong(&buffer, &pos);
	}
}

void SC_API(sc_getPlayers)(sc_GameServer *server, BOOL isGoldSrc)
{
	int sent = 0, recvd = 0;
	unsigned int pos = 0;
	unsigned char buffer[STEAM_PACKET_SIZE] = "";
	unsigned char *packet = NULL;
	sc_Players *last = NULL;
	int i = 0;
	char message[10] = "\xFF\xFF\xFF\xFF\x55\0xFF\0xFF\0xFF\0xFF";
	
	if (!server->challenge) sc_getChallenge(server);
	
	if (server->challenge) {
		message[5] = (char)(server->challenge & 0xFF);
		message[6] = (char)((server->challenge & 0xFF00) >> 8);
		message[7] = (char)((server->challenge & 0xFF0000) >> 16);
		message[8] = (char)((server->challenge & 0xFF000000) >> 24);
	}
	
	if (sent = send(server->socketUDP, &message, A2S_PLAYER_SIZE+4, 0) < 0) {
		fprintf(stderr, "Unable to send all data");
	}
	
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	if (sc_readLong(&buffer, &pos) == -2) {
		packet = sc_combineSplitPackets(server->socketUDP, &buffer, &pos, recvd, isGoldSrc);
		if (sc_readLong(packet, &pos) != -1) return;
	}
	if (!packet) packet = &buffer;
	
	if (sc_readByte(packet, &pos) != A2S_PLAYER_ACK) return;
	
	server->numPlayers = sc_readByte(packet, &pos);
	for (i = 0; i < server->numPlayers; i++) {
		sc_Players *player = malloc(sizeof(sc_Players));
		player->index = sc_readByte(packet, &pos);
		sc_readString(&player->name, packet, &pos);
		player->kills = sc_readLong(packet, &pos);
		player->time = sc_readFloat(packet, &pos);
		player->next = NULL;
		if (last) {
			last->next = player;
		} else {
			server->players = player;
		}
		last = player;
	}
	if (packet != &buffer) free(packet);
}

void SC_API(sc_getRules)(sc_GameServer *server, BOOL isGoldSrc)
{
	int sent = 0, recvd = 0;
	unsigned int pos = 0;
	unsigned char buffer[STEAM_PACKET_SIZE] = "";
	unsigned char *packet = NULL;
	sc_Rules *last = NULL;
	int i = 0;
	char message[10] = "\xFF\xFF\xFF\xFF\x56\0xFF\0xFF\0xFF\0xFF";
	
	if (!server->challenge) sc_getChallenge(server);
	
	if (server->challenge) {
		message[5] = (char)(server->challenge & 0xFF);
		message[6] = (char)((server->challenge & 0xFF00) >> 8);
		message[7] = (char)((server->challenge & 0xFF0000) >> 16);
		message[8] = (char)((server->challenge & 0xFF000000) >> 24);
	}
	
	if (sent = send(server->socketUDP, &message, A2S_RULES_SIZE+4, 0) < 0) {
		fprintf(stderr, "Unable to send all data");
	}
	
	recvd = recv(server->socketUDP, &buffer, STEAM_PACKET_SIZE, 0);
	if (sc_readLong(&buffer, &pos) == -2) {
		packet = sc_combineSplitPackets(server->socketUDP, &buffer, &pos, recvd, isGoldSrc);
		if (sc_readLong(packet, &pos) != -1) return;
	}
	if (!packet) packet = &buffer;
	
	if (sc_readByte(packet, &pos) != A2S_RULES_ACK) return;
	
	server->numRules = sc_readShort(packet, &pos);
	for (i = 0; i < server->numRules; i++) {
		sc_Rules *rule = malloc(sizeof(sc_Rules));
		sc_readString(&rule->name, packet, &pos);
		sc_readString(&rule->value, packet, &pos);
		rule->next = NULL;
		if (last) {
			last->next = rule;
		} else {
			server->rules = rule;
		}
		last = rule;
	}
	if (packet != &buffer) free(packet);
}

void SC_API(sc_updatePlayers)(sc_GameServer *server)
{
	
}

void SC_API(sc_updateRules)(sc_GameServer *server)
{
	
}


void SC_API(sc_freePlayers)(sc_Players *players)
{
	sc_Players *next = NULL;
	while (players) {
		next = players->next;
		free(players);
		players = next;
	}
}

void SC_API(sc_freeRules)(sc_Rules *rules)
{
	sc_Rules *next = NULL;
	while (rules) {
		next = rules->next;
		free(rules);
		rules = next;
	}
}

void SC_API(sc_freeGameServer)(sc_GameServer *server)
{
	server->numRules = 0;
	sc_freeRules(server->rules);
	server->numPlayers = 0;
	sc_freePlayers(server->players);
}
