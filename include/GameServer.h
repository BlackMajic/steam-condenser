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

struct SourceInfo {
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
};

struct Rules {
	char *name;
	char *value;
	struct Rules *next;
};

struct Players {
	byte index;
	byte *name;
	long kills;
	float time;
	struct Players *next;
};

struct GameServer {
	int socketUDP;
	int socketTCP;
	int ping;
	long challenge;
	struct addrinfo *addr;
	struct SourceInfo *info;
	byte numRules;
	struct Rules *rules;
	byte numPlayers;
	struct Players *players;
};

extern struct GameServer* getGameServerFromString(char *address);
extern struct GameServer* getGameServerFromAddress(struct addrinfo *address);

extern void getPing(struct GameServer *server);
extern void getServerInfo(struct GameServer *server);
extern void getChallenge(struct GameServer *server);
extern void getPlayers(struct GameServer *server);
extern void getRules(struct GameServer *server);

extern void updatePlayers(struct GameServer *server);
extern void updateRules(struct GameServer *server);

extern void freePlayers(struct Players *players);
extern void freeRules(struct Rules *players);
extern void freeGameServer(struct GameServer *server);

#endif
