/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG // change to NDEBUG on release

#include "steam-condenser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printServers();
void printServerInfo(const char *address);

int main(int argc, char *argv[])
{
	char addr[ADDRSTRLEN] = "92.83.108.33:15868"; //"208.64.37.125:27018";//"216.52.148.77:27015";
	
	if (SC_ERROR(sc_init())) {
		printf("error");
	} else {
		printf("good");
	}
	
	printServers();
	
	//printServerInfo(addr, FALSE);
	system("pause");
	
	sc_end();
	return 0;
}

void printServerInfo(const char *address, BOOL goldSrc)
{
	sc_GameServer *server = NULL;
	sc_Rules *rule = NULL;
	sc_Players *player = NULL;
	server = sc_getGameServerFromString(address);
	rule = server->rules;
	player = server->players;
	sc_getPing(server);
	sc_getServerInfo(server, goldSrc);
	if (!strcmp(server->info.hostname, "")) {
		sc_freeGameServer(server);
		return;
	}
	
	printf("Server type: %d\n", server->info.type);
	printf("Network Protocol Version: %d\n", server->info.npVersion);
	printf("Server name: %s\n", server->info.hostname);
	printf("Map: %s\n", server->info.map);
	printf("Game Dir: %s\n", server->info.gameDir);
	printf("Game Description: %s\n", server->info.gameDesc);
	printf("AppID: %d\n", server->info.appID);
	if (server->info.appID2)
		printf("AppID2: %d\n", server->info.appID2);
	printf("Players: %d/%d (%d Bots)\n", server->info.numPlayers, server->info.maxPlayers, server->info.numBots);
	printf("Dedicated: %c\n", server->info.dedicated);
	printf("OS: %c\n", server->info.os);
	printf("Locked: %d\n", server->info.password);
	printf("Secure: %d\n", server->info.secure);
	printf("Game Version: %s\n", server->info.gameVersion);
	if (server->info.port)
		printf("Port: %d\n", server->info.port);
	if (server->info.steamID)
		printf("SteamID: %u\n", server->info.steamID);
	if (server->info.specPort)
		printf("Spectator Port and Name: %d - %s\n", server->info.specPort, server->info.specName);
	if (server->info.tags)
		printf("Tags: %s\n", server->info.tags);
	printf("\n");
	
	sc_getRules(server, goldSrc);
	rule = server->rules;
	printf("Rules:\n");
	while (rule) {
		printf("%s = %s\n", rule->name, rule->value);
		rule = rule->next;
	}
	
	sc_getPlayers(server, goldSrc);
	player = server->players;
	printf("Players:\n");
	while (player) {
		printf("%d - %s - %d - %2f\n", player->index, player->name, player->kills, player->time);
		player = player->next;
	}
	
	sc_freeGameServer(server);
}

void printServers()
{
	sc_MasterServer	*master = NULL;
	sc_ServerList	*server = NULL;
	unsigned int	i = 0;
	
	master = sc_getMasterServer(SOURCE_MASTER);
	sc_getServers(master, REGION_US_EAST, "");
	
	server = master->servers;
	while (server != NULL) {
		char addr[ADDRSTRLEN] = "";
		strcat(addr, server->address);
		strcat(addr, ":");
		strcat(addr, server->port);
		printf("%s:%s\n", server->address, server->port);
		printServerInfo(addr, FALSE);
		server = server->next;
		
		i++;
	}
	printf("\nFound %d Servers\n\n", i);
	
	// do some cleanup...
	sc_freeMasterServer(master);
}

#ifdef __cplusplus
}
#endif
