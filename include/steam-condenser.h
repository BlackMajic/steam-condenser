/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#ifndef STEAM_CONDENSER_H
#define STEAM_CONDENSER_H

#define DEBUG // change to NDEBUG on release

#ifdef __cplusplus
typedef enum { false, true } bool;
extern "C" {
#endif

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WIN32_WINNT 0x0501
	#define WINVER 0x0501
	#include <winsock2.h>
	#include <ws2tcpip.h>
	//#if NTDDI_VERSION < NTDDI_LONGHORN
		extern const char* inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
		extern int inet_pton(int af, const char *src, void *dst);
	//#endif
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <bzlib.h>

typedef char byte;

/*******************************************************************************
 * Generic Function Declarations
 ******************************************************************************/
extern int sc_init();
extern void sc_end();
extern byte readByte(char *buffer, int *position);
extern short readShort(char *buffer, int *position);
extern long readLong(char *buffer, int *position);
extern float readFloat(char *buffer, int *position);
extern long long readLongLong(char *buffer, int *position);
extern char* readString(char *buffer, int *position, int continueFrom);
/*******************************************************************************
 * Custom Includes
 ******************************************************************************/
#include "MasterServer.h"
#include "GameServer.h"

#ifdef __cplusplus
}
#endif

#endif // STEAM_CONDENSER_H
