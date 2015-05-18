#include "NetworkManager.h"

NetworkManager::NetworkManager(HWND hWnd)
{
	// initialize the buffers that will be sent to the server
	transBuf.posX = 0;
	transBuf.posY = 0;
	transBuf.posZ = 0;
	transBuf.rotX = 0;
	transBuf.rotY = 0;
	transBuf.rotZ = 0;
	transBuf.rotW = 0;

	numPlayers = 1;
	playerIndex = 0;

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

	// get the player index from the server
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	playerIndex = atoi(recvbuf);
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

		// Send player's transform info
		for(int i = 0; i < 6; i++)
		{
			UpdateTransformBuffer(sendObjects[i]->geometry->GetPosition(), sendObjects[i]->geometry->GetRotation());
			iResult = send( ConnectSocket, reinterpret_cast<char*>(&transBuf), sizeof(transBuf), 0 );
			
			if (iResult == SOCKET_ERROR) 
			{
				//printf("send failed with error: %d\n", WSAGetLastError());
			}
			else
			{
				//printf("Bytes Sent: %ld\n", iResult);
			}

			// Receive until the peer closes the connection
			if(numPlayers > 1)//for(int j = 0; j < numPlayers-1; j++)
			{
				iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
				Transform* newTrans = reinterpret_cast<Transform*>(recvbuf);
				receiveObjects[i]->Translation(XMFLOAT3(newTrans->posX, newTrans->posY, newTrans->posZ));
				receiveObjects[i]->Rotation(XMFLOAT4(newTrans->rotX, newTrans->rotY, newTrans->rotZ, newTrans->rotW));

				if ( iResult > 0 )
				{
					//printf("Bytes received: %d\n", iResult);
				}
				else if ( iResult == 0 )
				{
					//printf("Connection closed\n");
				}
				else
				{
					//printf("recv failed with error: %d\n", WSAGetLastError());
				}
			}
		}
	}
}

int NetworkManager::GetPlayerIndex()
{
	return playerIndex;
}

void NetworkManager::UpdateTransformBuffer(XMFLOAT3 pos, XMFLOAT4 rot)
{
	transBuf.posX = pos.x;
	transBuf.posY = pos.y;
	transBuf.posZ = pos.z;
	transBuf.rotX = rot.x;
	transBuf.rotY = rot.y;
	transBuf.rotZ = rot.z;
	transBuf.rotW = rot.w;
}