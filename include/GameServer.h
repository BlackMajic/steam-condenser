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

#define A2A_PING							"\xFF\xFF\xFF\xFF\x69"
#define A2A_PING_SIZE						5
// reply with 0x6A \0 (goldsrc), 0x6A '00000000000000' (source)
#define A2S_INFO							"\xFF\xFF\xFF\xFF\x54Source Engine Query"
#define A2S_INFO_SIZE						25
// reply varies
#define A2S_SERVERQUERY_GETCHALLENGE		"\xFF\xFF\xFF\xFF\x57"
#define A2S_SERVERQUERY_GETCHALLENGE_SIZE	5
// reply with 0x41 long [challenge]
#define A2S_PLAYER							"\xFF\xFF\xFF\xFF\x55" //+challenge
#define A2S_PLAYER_SIZE						5
// 0x44 byte [numplayers]
// then, numplayers*(byte string long float) [index, name, kills, time]
#define A2S_RULES							"\xFF\xFF\xFF\xFF\x56" //+challenge
#define A2S_RULES_SIZE						5
// 0x45 byte [numrules]
// then, numrules*(string string) [name value]

typedef struct sc_ModInfo {
	char urlInfo[128];
	char urlDL[128];
	long version;
	long dlSize;
	byte nil;
	byte svOnly;
	byte clDLL;
} sc_ModInfo;

typedef struct sc_ServerInfo {
	char hostname[64];
	char gameDesc[64];
	char gameDir[32];
	char map[32];
	byte type;
	byte npVersion;						// Network Protocol Version
	byte numPlayers;
	byte maxPlayers;
	byte numBots;
	byte dedicated;						// l/d/p - listen, dedicated, HLTV
	byte os;							// l/w   - linux, windows
	byte password;						// 0x01  - password required
	byte secure;						// 0x01  - VAC Enabled
	
	union {
		struct sc_SourceInfo {
			char gameVersion[32];		// Untested size, but come on.
			short appID;
			union {
				struct {				// Standard Source server
					char tag[128];		// if EDF & 0x20
					char specName[64];	
					long long steamID;	// if EDF & 0x10
					short port;			// if EDF & 0x80
					short specPort;		// if EDF & 0x40, followed by specName
					short appID2;		// if EDF & 0x01 - appID again plus a bunch of NULL bytes
					byte EDF;			// Extra Data Flag
				};
				struct {				// The Ship
					byte gameMode;		// 0 - Hunt,		1 - Elmination,	2 - Duel
										// 3 - Deathmatch,	4 - Team VIP,	5 - Team Elimination
					byte witnessCount;	// Min winesses to be arrested
					byte witnessTime;	// Time arrested after being witnessed
				};
			};
		};
		struct sc_GoldSrcInfo {	
			sc_ModInfo modInfo;			// Additional mod info (if isMod is set)
			char gameIP[ADDRSTRLEN];
			byte isMod;					// if 0x01, modData should be populated
		};
	};
} sc_ServerInfo;

typedef struct sc_Rules {
	char value[128];					// Assuming same length as tag
	char name[64];						// Saw no CVARs longer than 50 chars
	struct sc_Rules *next;
} sc_Rules;

typedef struct sc_Players {
	char name[32];
	long kills;
	float time;
	byte index;
	struct sc_Players *next;
} sc_Players;

typedef struct sc_GameServer {
	sc_ServerInfo	info;
	long			challenge;
	int				socketUDP;
	int				socketTCP;
	int				ping;
	sc_Rules		*rules;
	sc_Players		*players;
	byte			numRules;
	byte			numPlayers;
} sc_GameServer;

SC_EXTERN sc_GameServer* SC_API(sc_getGameServerFromString)	(const char *address);
SC_EXTERN sc_GameServer* SC_API(sc_getGameServer)			(const char *address, const char *port);

SC_EXTERN int  SC_API(sc_getPing)		(sc_GameServer *server);
SC_EXTERN void SC_API(sc_getServerInfo)	(sc_GameServer *server);
SC_EXTERN void SC_API(sc_getChallenge)	(sc_GameServer *server);
SC_EXTERN void SC_API(sc_getPlayers)	(sc_GameServer *server);
SC_EXTERN void SC_API(sc_getRules)		(sc_GameServer *server);

SC_EXTERN void SC_API(sc_updatePlayers)	(sc_GameServer *server);
SC_EXTERN void SC_API(sc_updateRules)	(sc_GameServer *server);

SC_EXTERN void SC_API(sc_freePlayers)	(sc_Players *players);
SC_EXTERN void SC_API(sc_freeRules)		(sc_Rules *players);
SC_EXTERN void SC_API(sc_freeGameServer)(sc_GameServer *server);

#ifdef __cplusplus
}
#endif

#endif
