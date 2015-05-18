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

using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "42779"

char ipAddress[15];
int numPlayers;

void closeClientSockets(SOCKET* clients, int numClients)
{
	for(int i = 0; i < numClients; i++)
	{
		closesocket(clients[i]);
	}
}

int main(void) 
{
	numPlayers = 1;
	// get the ipAddress and number of players from the config file
	ifstream fileReader;
	fileReader.open("../config.txt");
	fileReader >> ipAddress;
	char temp[2];
	fileReader >> temp;
	numPlayers = atoi(temp);

	// do the networking stuff
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET* ClientSocket = new SOCKET[numPlayers];
	for(int i = 0; i < numPlayers; i++)
	{
		ClientSocket[i] = INVALID_SOCKET;
	}

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
	for(int i = 0; i < numPlayers; i++)
	{
		ClientSocket[i] = accept(ListenSocket, NULL, NULL);
		if (ClientSocket[i] == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		char pIndex[2];
		_itoa_s(i, pIndex, 10);
		iSendResult = send( ClientSocket[i], pIndex, sizeof(pIndex), 0 );
	}

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    do {
		// receive and send out if the players are dead
		for(int i = 0; i < numPlayers; i++)
		{
			iResult = recv(ClientSocket[i], recvbuf, recvbuflen, 0);
			for(int j = 0; j < numPlayers; j++)
			{
				if(i != j)
				{
					iSendResult = send( ClientSocket[i], recvbuf, iResult, 0 );
				}
			}
		}

		// get the game loop messages from the clients
		for(int k = 0; k < 6; k++)
		{
			for(int i = 0; i < numPlayers; i++)
			{
				// get the clients' transforms
				iResult = recv(ClientSocket[i], recvbuf, recvbuflen, 0);
				if (iResult > 0) 
				{
					printf("Bytes received: %d\n", iResult);

					// send the transfrms to the other clients on the server
					for( int j = 0; j < numPlayers; j++)
					{
						if(i != j)
						{
							iSendResult = send( ClientSocket[j], recvbuf, iResult, 0 );
							if (iSendResult == SOCKET_ERROR) 
							{
								printf("send failed with error: %d\n", WSAGetLastError());
								closeClientSockets(ClientSocket, numPlayers);
								WSACleanup();
								return 1;
							}
							else
							{
								//printf("Sent %d Bytes to %i from %i\n", iSendResult, j, i);
							}
						}
					}
				}
				else if (iResult == 0)
					printf("Connection closing...\n");
				else if(iResult < 0)
				{
					printf("recv failed with error: %d\n", WSAGetLastError());
					closeClientSockets(ClientSocket, numPlayers);
					WSACleanup();
					return 1;
				}
			}
		}
    } while (iResult > 0);

    // shutdown the connection since we're done
	for(int i = 0; i < numPlayers; i++)
	{
		iResult = shutdown(ClientSocket[i], SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closeClientSockets(ClientSocket, numPlayers);
			WSACleanup();
			return 1;
		}
	}

    // cleanup
    closeClientSockets(ClientSocket, numPlayers);
    WSACleanup();

    return 0;
}