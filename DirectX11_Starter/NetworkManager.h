#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "42779"

#define WM_ONSOCKET WM_USER+1

class NetworkManager
{
public:
	NetworkManager(HWND);
	~NetworkManager(void);

	void Update();

private:
	char ipAddress[15];
	int numPlayers;

	WSADATA wsaData;
    SOCKET ConnectSocket;
    struct addrinfo *result;
    struct addrinfo *ptr;
    struct addrinfo hints;
    char *sendbuf;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen;
};

