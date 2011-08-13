/*******************************************************************************
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 * 
 * Copyright © 2010 Tim McLennan <tim@nothingness.ca>
 * 
 * Source code is part of the steam-condenser project
 * http://koraktor.github.com/steam-condenser
 ******************************************************************************/
#include "steam-condenser.h"

#include <stdlib.h>
#include <stdio.h>

int SC_API(sc_init)()
{
	int ret = 0;
#ifdef WIN32
	struct WSAData wsaData;
	
	if (ret = WSAStartup(MAKEWORD(2,2), &wsaData)) {
		SC_ERRORMSG("WSAStartup Failed", ret);
	}
#endif
	return ret;
}

void SC_API(sc_trace)(const char *msg, int id, const char *file, unsigned int line)
{
	#ifdef _DEBUG
		#ifdef WIN32
			char message[4096] = "";
			sprintf(message, "%s(%u): Error %d: %s\n", file, line, id, msg);
			OutputDebugString(message);
		#else
			fprintf(stderr,  "%s(%u): Error %d: %s\n", file, line, id, msg);
		#endif
	#endif
}

// seperate address into address:port, then pass along
int SC_API(sc_openSocketAddr)(const char *address, int socktype)
{
	char *serv = strdup(address);
	char *addr = strtok(serv, "[");
	char *port = "";
	int sock = 0;
	if (strcmp(addr, serv)) {
		// IPv6 [::1]:0
		addr = strtok(addr, "]");
		port = strtok(NULL, ":");
	} else {
		// IPv4 or named 127.0.0.1:0 / localhost:0
		addr = strtok(serv, ":");
		port = strtok(NULL, ":");
	}
	sock = sc_openSocketAddrPort(addr, port, socktype);
	free(serv);
	return sock;
}

// connect() to address:port using socktype
int SC_API(sc_openSocketAddrPort)(const char *address, const char *port, int socktype)
{
	int sock = 0, conn = 0;
	struct addrinfo hints, *servers, *server;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= socktype;
	hints.ai_flags		= AI_PASSIVE;
	
	if (getaddrinfo(address, port, &hints, &servers) != 0) {
		SC_ERRORMSG("getaddrinfo failed while trying to connect to master server", SC_GETADDRINFO_FAILED);
		return SC_GETADDRINFO_FAILED;
	}
	
	for (server = servers; server != NULL; server = server->ai_next) {
		sock = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
		if (sock != -1) {
			if (conn = connect(sock, server->ai_addr, server->ai_addrlen) != -1) {
				#ifdef WIN32
				DWORD timeout = 2000;
				if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(long)*2) == -1) {
					SC_ERRORMSG("Failed to set outgoing timeout", SC_SOCKOPT_FAILED);
				}
				if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(long)*2) == -1) {
					SC_ERRORMSG("Failed to set incoming timeout", SC_SOCKOPT_FAILED);
				}
				#else
				struct timeval timeout;
				timeout.tv_sec = 2;
				timeout.tv_usec = 0;
				if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(long)*2) == -1) {
					SC_ERRORMSG("Failed to set outgoing timeout", SC_SOCKOPT_FAILED);
				}
				if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(long)*2) == -1) {
					SC_ERRORMSG("Failed to set incoming timeout", SC_SOCKOPT_FAILED);
				}
				#endif
				
				break;
			} else {
				SC_ERRORMSG("Failed to connect() to master server", SC_CONNECT_FAILED);
			}
		} else {
			SC_ERRORMSG("Failed to create socket to master server", SC_SOCKET_FAILED);
		}
	}
	freeaddrinfo(servers);
	if (conn == -1) return SC_CONNECT_FAILED;
	if (sock == -1) return SC_SOCKET_FAILED;
	return sock;
}

void SC_API(sc_closeSocket)(int *socket)
{
	shutdown(*socket, 2);
#ifdef WIN32
	closesocket(*socket);
#else
	close(*socket);
#endif
	socket = NULL;
}

void SC_API(sc_end)()
{
#ifdef WIN32
	WSACleanup();
#endif
}

/**
 * Valve stores everything in little endian except the serverlist from master server.
 */
byte SC_API(sc_readByte)(unsigned char *buffer, int *position)
{
	return buffer[(*position)++];
}

short SC_API(sc_readShort)(unsigned char *buffer, int *position)
{
	return sc_readByte(buffer, position) & 0xFF | ((sc_readByte(buffer, position) & 0xFF) << 8);
}

long SC_API(sc_readLong)(unsigned char *buffer, int *position)
{
	return sc_readShort(buffer, position) & 0xFFFF | ((sc_readShort(buffer, position) & 0xFFFF) << 16);
}

float SC_API(sc_readFloat)(unsigned char *buffer, int *position)
{
	char f[4] = "";
	int i = 0;
	for (i = 0; i < 4; i++) {
		f[i] = sc_readByte(buffer, position);
	}
	return *(float*)f;
}

long long SC_API(sc_readLongLong)(unsigned char *buffer, int *position)
{
	return sc_readLong(buffer, position) & 0xFFFFFFFF | ((long long)(sc_readLong(buffer, position) & 0xFFFFFFFF) << 32);
}

void SC_API(sc_readString)(char *dest, unsigned int destLength, unsigned char *buffer, int *position)
{
	unsigned int i = 0;
	while (buffer[(*position)] != 0x00 && i < destLength) {
		dest[i++] = buffer[(*position)++];
	}
	if (buffer[(*position)] != 0x00) {
		SC_ERRORMSG("Source string longer than destination.", SC_OK);
		while (buffer[(*position)++] != 0x00) {}
	}
	dest[i] = 0x00;
	(*position)++;
}


/**
 * Windows implementations of inet_ntop and inet_pton
 * original source: http://www.mail-archive.com/users@ipv6.org/msg02107.html
 */
#ifdef WIN32
#if (NTDDI_VERSION < NTDDI_VISTA)
const char* inet_ntop(int af, const void *src, char *dst, socklen_t cnt)
{
	if (af == AF_INET) {
		struct sockaddr_in in;
		memset(&in, 0, sizeof(in));
		in.sin_family = AF_INET;
		memcpy(&in.sin_addr, src, sizeof(struct in_addr));
		getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
		return dst;
	} else if (af == AF_INET6) {
		struct sockaddr_in6 in;
		memset(&in, 0, sizeof(in));
		in.sin6_family = AF_INET6;
		memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
		getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
		return dst;
	}
	return NULL;
}

int inet_pton(int af, const char *src, void *dst)
{
	struct addrinfo hints, *res, *ressave;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = af;
	
	if (getaddrinfo(src, NULL, &hints, &res) != 0) {
		fprintf(stderr, "Couldn't resolve host %s\n", src);
		return -1;
	}
	
	ressave = res;
	
	while (res) {
		memcpy(dst, res->ai_addr, res->ai_addrlen);
		res = res->ai_next;
	}
	
	freeaddrinfo(ressave);
	return 0;
}
#endif // HAVE_INET_PTON
#endif // ifdef WIN32
