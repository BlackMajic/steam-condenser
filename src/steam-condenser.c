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

int sc_init()
{
	int ret = 0;
#ifdef WIN32
	WSADATA wsaData;   // if this doesn't work
	//WSAData wsaData; // then try this instead
	
	if (ret = WSAStartup(MAKEWORD(2,2), &wsaData)) {
		fprintf(stderr, "WSAStartup failed.\n");
		exit(1);
	}
#endif
	return ret;
}

void sc_end()
{
#ifdef WIN32
	WSACleanup();
#endif
}

/**
 * Valve stores everything in little endian except the serverlist from master server.
 */
byte readByte(char *buffer, int *position)
{
	return buffer[*position++];
}

short readShort(char *buffer, int *position)
{
	return readByte(buffer, position) | (readByte(buffer, position) << 8);
}

long readLong(char *buffer, int *position)
{
	return readShort(buffer, position) | (readShort(buffer, position) << 16);
}

float readFloat(char *buffer, int *position)
{
	return 1.0;
}

long long readLongLong(char *buffer, int *position)
{
	return readLong(buffer, position) | (readLong(buffer, position) << 32);
}

char* readString(char *buffer, int *position, int continueFrom)
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
//#if NTDDI_VERSION < NTDDI_LONGHORN
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
//#endif // if NTDDI_VERSION < NTDDI_LONGHORN
#endif // ifdef WIN32
