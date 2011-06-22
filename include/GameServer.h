/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#ifndef GAMESERVER_H
#define GAMESERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "steam-condenser.h"

#define STEAM_PACKET_SIZE				1400
#define A2A_PING						0x69
// reply with 0x6A \0 (goldsrc), 0x6A '00000000000000' (source)
#define A2S_INFO						"\xFF\xFF\xFF\xFF\x54Source Engine Query"
// reply varies
#define A2S_SERVERQUERY_GETCHALLENGE	"\xFF\xFF\xFF\xFF\x57"
// reply with 0x41 long [challenge]
#define A2S_PLAYER						"\xFF\xFF\xFF\xFF\x55" //+challenge
// 0x44 byte [numplayers]
// then, numplayers*(byte string long float) [index, name, kills, time]
#define A2S_RULES						"\xFF\xFF\xFF\xFF\x56" //+challenge
// 0x45 byte [numrules]
// then, numrules*(string string) [name value]

typedef struct sc_SourceInfo {
	byte npVersion;
	char *name;
	char *map;
	char *gameDir;
	char *gameDesc;
	short appID;
	byte numPlayers;
	byte maxPlayers;
	byte numBots;
	byte dedicated; // l, d, or p (listen, dedicated, tv)
	byte os; // l, or w (linux, windows)
	byte password;
	byte secure;
	char *gameVersion;
	byte EDF;
	short port; // if EDF & 0x80
	long long steamID; // if EDF & 0x10
	short specPort; // if EDF & 0x40, followed by specName
	char *specName;
	char *tag; // if EDF & 0x20, future use
	/*
	short appID;
	char *unknown;
	*/
} sc_SourceInfo;

typedef struct sc_Rules {
	char *name;
	char *value;
	struct sc_Rules *next;
} sc_Rules;

typedef struct sc_Players {
	byte index;
	byte *name;
	long kills;
	float time;
	struct sc_Players *next;
} sc_Players;

typedef struct sc_GameServer {
	int socketUDP;
	int socketTCP;
	int ping;
	long challenge;
	struct addrinfo *addr;
	struct sc_SourceInfo *info;
	byte numRules;
	struct sc_Rules *rules;
	byte numPlayers;
	struct sc_Players *players;
} sc_GameServer;

SC_EXTERN sc_GameServer* SC_API(sc_getGameServerFromString)	(char *address);
SC_EXTERN sc_GameServer* SC_API(sc_getGameServerFromAddress)(struct addrinfo *address);

SC_EXTERN void SC_API(sc_getPing)		(struct GameServer *server);
SC_EXTERN void SC_API(sc_getServerInfo)	(struct GameServer *server);
SC_EXTERN void SC_API(sc_getChallenge)	(struct GameServer *server);
SC_EXTERN void SC_API(sc_getPlayers)	(struct GameServer *server);
SC_EXTERN void SC_API(sc_getRules)		(struct GameServer *server);

SC_EXTERN void SC_API(sc_updatePlayers)	(struct GameServer *server);
SC_EXTERN void SC_API(sc_updateRules)	(struct GameServer *server);

SC_EXTERN void SC_API(sc_freePlayers)	(struct Players *players);
SC_EXTERN void SC_API(sc_freeRules)		(struct Rules *players);
SC_EXTERN void SC_API(sc_freeGameServer)(struct GameServer *server);

#ifdef __cplusplus
}
#endif

#endif
