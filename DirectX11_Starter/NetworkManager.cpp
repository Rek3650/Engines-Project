#include "NetworkManager.h"

NetworkManager::NetworkManager(HWND hWnd)
{
	numPlayers = 1;
	// get the ipAddress and number of players from the config file
	std::ifstream fileReader;
	fileReader.open("../GameServer/config.txt");
	fileReader >> ipAddress;
	char temp[2];
	fileReader >> temp;
	numPlayers = atoi(temp);

	// set up the socket and connect to the server
    ConnectSocket = INVALID_SOCKET;
    result = NULL;
    ptr = NULL;
    sendbuf = "this is a test";
    recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) 
	{
        printf("WSAStartup failed with error: %d\n", iResult);
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ipAddress, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) 
	{
        printf("getaddrinfo failed with error: %d\n", iResult);
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) 
	{
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) 
		{
            printf("socket failed with error: %ld\n", WSAGetLastError());
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
		{
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) 
	{
        printf("Unable to connect to server!\n");
    }
}


NetworkManager::~NetworkManager(void)
{
	// shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
	{
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}

void NetworkManager::Update()
{
	if(numPlayers > 0)
	{
		// Send information about this client to the server
		iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
		if (iResult == SOCKET_ERROR) 
		{
			printf("send failed with error: %d\n", WSAGetLastError());
		}
		else
		{
			//printf("Bytes Sent: %ld\n", iResult);
		}

		// Receive until the peer closes the connection
		for(int i = 0; i < numPlayers-1; i++)
		{
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if ( iResult > 0 )
			{
				printf("Bytes received: %d\n", iResult);
			}
			else if ( iResult == 0 )
				printf("Connection closed\n");
			else
				printf("recv failed with error: %d\n", WSAGetLastError());
		}
	}
}