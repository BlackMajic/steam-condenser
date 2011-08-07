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

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#define ADDRSTRLEN			65		// enough space for ip and port
#define STEAM_PACKET_SIZE	1400	// max packet size a server will respond with (plus headers)
typedef char byte;

enum sc_errors {
	SC_OK,
	SC_STARTUP_FAILED,
	SC_GETADDRINFO_FAILED,
	SC_SOCKET_FAILED,
	SC_CONNECT_FAILED,
	SC_SOCKOPT_FAILED,
	// Master Server
	
	// Game Server
};

#ifndef SC_IMPORT
	#define SC_EXPORT
#endif

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#ifdef SC_EXPORT
		#define SC_EXTERN		extern
		#define SC_API(func)	WINAPI func
	#else
		#define SC_EXTERN
		#define SC_API(func)	(WINAPI * func)
	#endif
	#ifndef HAVE_INET_NTOP
		const char*	inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
	#endif
	#ifndef HAVE_INET_PTON
		int			inet_pton(int af, const char *src, void *dst);
	#endif
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#define SC_EXTERN		extern
	#define SC_API(func)	func
#endif

/*******************************************************************************
 * Generic Function Declarations
 ******************************************************************************/
SC_EXTERN int	SC_API(sc_init)					();
SC_EXTERN void	SC_API(sc_end)					();
int				SC_API(sc_openSocketAddr)		(const char *address, int socktype);
int				SC_API(sc_openSocketAddrPort)	(const char *address, const char *port, int socktype);
void			SC_API(sc_closeSocket)			(int *socket);
byte			SC_API(sc_readByte)				(char *buffer, int *position);
short			SC_API(sc_readShort)			(char *buffer, int *position);
long			SC_API(sc_readLong)				(char *buffer, int *position);
float			SC_API(sc_readFloat)			(char *buffer, int *position);
long long		SC_API(sc_readLongLong)			(char *buffer, int *position);
void			SC_API(sc_readString)			(char *dest, unsigned int destLength, char *buffer, int *position);
void			SC_API(sc_trace)				(const char *msg, unsigned int id, const char *file, unsigned int line);

#ifdef DEBUG
	#define SC_ERROR(id)			((id) != SC_OK)
	#define SC_ERRORMSG(msg, id)	sc_trace((msg), (id), __FILE__, __LINE__)
#else
	#define SC_ERROR(id)
	#define SC_ERRORMSG(msg, id)
#endif

/*******************************************************************************
 * Custom Includes
 ******************************************************************************/
#include "MasterServer.h"
#include "GameServer.h"

#ifdef __cplusplus
}
#endif

#endif // STEAM_CONDENSER_H
