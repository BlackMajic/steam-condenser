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
		fprintf(stderr, "WSAStartup failed.\n");
		exit(1);
	}
#endif
	return ret;
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
	int sock = 0;
	struct addrinfo hints, *servers, *server;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= socktype;
	hints.ai_flags		= AI_PASSIVE;
	
	if (getaddrinfo(address, port, &hints, &servers) != 0) {
		perror("getaddrinfo");
		exit(2);
	}
	
	for (server = servers; server != NULL; server = server->ai_next) {
		sock = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
		if (sock == -1) {
			perror("Sockout Error");
		} else {
			if (connect(sock, server->ai_addr, server->ai_addrlen) == -1) {
				perror("Connect Error");
			} else {
				break;
			}
		}
	}
	if (sock == -1) exit(3);
	freeaddrinfo(servers);
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
byte SC_API(sc_readByte)(char *buffer, int *position)
{
	return buffer[*position++];
}

short SC_API(sc_readShort)(char *buffer, int *position)
{
	return sc_readByte(buffer, position) | (sc_readByte(buffer, position) << 8);
}

long SC_API(sc_readLong)(char *buffer, int *position)
{
	return sc_readShort(buffer, position) | (sc_readShort(buffer, position) << 16);
}

float SC_API(sc_readFloat)(char *buffer, int *position)
{
	return sc_readLong(buffer, position);
}

long long SC_API(sc_readLongLong)(char *buffer, int *position)
{
	return sc_readLong(buffer, position) | (sc_readLong(buffer, position) << 32);
}

char* SC_API(sc_readString)(char *buffer, int *position, int continueFrom)
{
	int maxLen = STEAM_PACKET_SIZE - *position;
	char *ret = calloc(sizeof(char), maxLen);
	int i = 0;
	if (continueFrom) i = continueFrom;
	while (buffer[*position] != '\0' && (i-continueFrom) <= maxLen) {
		ret[i++] = buffer[*position++];
	}
	realloc(ret, strlen(ret)+1);
	return ret;
}



/**
 * Windows implementations of inet_ntop and inet_pton
 * original source: http://www.mail-archive.com/users@ipv6.org/msg02107.html
 */
#ifdef WIN32
#ifndef HAVE_INET_NTOP
#define HAVE_INET_NTOP
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
#endif // HAVE_INET_NTOP

#ifndef HAVE_INET_PTON
#define HAVE_INET_PTON
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
