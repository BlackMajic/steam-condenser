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

#define SC_OK					(0)
#define SC_STARTUP_FAILED		(-1)
#define SC_GETADDRINFO_FAILED	(-2)
#define SC_SOCKET_FAILED		(-3)
#define SC_CONNECT_FAILED		(-4)
#define SC_SOCKOPT_FAILED		(-5)
#define SC_SEND_ERROR			(-6)
#define SC_RECV_ERROR			(-7)
#define SC_BAD_REQ_ID			(-8)
#define SC_BAD_PACKET_NUMBER	(-9)
#define SC_BAD_CHECKSUM			(-10)
#define SC_MALFORMED_PACKET		(-11)
#define SC_MEMORY_ERROR			(-12)
#define SC_PROTO_ERROR			(-13)

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
	#ifndef NTDDI_VISTA
		#define NTDDI_VISTA		0x06000000
	#endif
	#if (NTDDI_VERSION < NTDDI_VISTA)
		const char*	inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
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
byte			SC_API(sc_readByte)				(unsigned char *buffer, int *position);
short			SC_API(sc_readShort)			(unsigned char *buffer, int *position);
long			SC_API(sc_readLong)				(unsigned char *buffer, int *position);
float			SC_API(sc_readFloat)			(unsigned char *buffer, int *position);
long long		SC_API(sc_readLongLong)			(unsigned char *buffer, int *position);
void			SC_API(sc_readString)			(char *dest, unsigned int destLength, unsigned char *buffer, int *position);
void			SC_API(sc_trace)				(const char *msg, int id, const char *file, unsigned int line);

#ifdef _DEBUG
	#define SC_ERROR(id)			((id) < SC_OK)
	#define SC_ERRORMSG(msg, id)	sc_trace(msg, id, __FILE__, __LINE__)
#else
	#define SC_ERROR(id)			(id)
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
