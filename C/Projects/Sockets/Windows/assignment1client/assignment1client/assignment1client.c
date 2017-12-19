// assignment1client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

#include <winsock.h>

#include <windows.h>

#include <io.h>
#include <string.h>
#include <sys/types.h>
#include <process.h>


// DECLARATIONS
// Error trapping signals
#define SIGINT 2
#define SIGKILL 9
#define SIGQUIT 34

// SOCKETS
SOCKET sock, client;

// Cleanup sockets
void s_handle(int s)
{
	if (sock)
		closesocket(sock);
	if (client)
		closesocket(client);
	WSACleanup();
	printf("EXIT SIGNAL: %d\n", s);
	exit(0);
}

void s_cl(char *a, int x)
{
	printf("%d", x);
	s_handle(x + 1000);
}

void main()
{
	//Declarations
	int res, n, i = 1;
	char buf[2048];
	char buf2[2048];
	WSADATA data;

	signal(SIGINT, s_handle);
	signal(SIGKILL, s_handle);
	signal(SIGQUIT, s_handle);

	printf("\t\tCLIENT STARTED");

	// Set up socket
	struct sockaddr_in ser;
	struct sockaddr addr;

	ser.sin_family = AF_INET;
	ser.sin_port = htons(2809);					
	ser.sin_addr.s_addr = inet_addr("127.0.0.1");		

	memcpy(&addr, &ser, sizeof(SOCKADDR_IN));

	// Start Winsock
	res = WSAStartup(MAKEWORD(1, 1), &data);	
	printf("\n\nWSAStartup");

	if (res != 0)
		s_cl("WSAStarup failed", WSAGetLastError());

	// Create Socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		
	if (sock == INVALID_SOCKET)
		s_cl("Invalid Socket ", WSAGetLastError());
	else if (sock == SOCKET_ERROR)
		s_cl("Socket Error ", WSAGetLastError());
	else
		printf("Socket Established\n");

	// Connect to server
	res = connect(sock, &addr, sizeof(addr));	
	if (res != 0)
	{
		s_cl("Cannot connect to server", res);
	}
	else
	{
		printf("Connected to Server...\n");
		memcpy(&ser, &addr, sizeof(SOCKADDR));
	}
	
	// Receive connection accepted command from server
	n = recv(sock, buf2, sizeof(buf2), 0);
	if (n < 0)
	{
		printf("Could not confirmation from server...\n");
	}
	memset((void *)buf2, 0, sizeof(buf2));

	// Loop through commands to send to server
	while (1)
	{
		gets(buf);
		printf("Sending command '%s'", buf);

		// Send command to server
		res = send(sock, buf, strlen(buf), 0);

		puts("\n");

		if (res == 0)
		{
			printf("\nServer terminated connection\n");
			closesocket(client);
			client = 0;
			break;
		}
		else if (res == SOCKET_ERROR)
		{
			printf("Socket error\n");
			s_handle(res);
			break;
		}

		memset((void *)buf, 0, sizeof(buf));

		// Get output from server
		res = recv(sock, buf2, sizeof(buf2), 0);

		if (res > 0)
		{
			printf("Received String: %s", buf2);
		}

		memset((void *)buf2, 0, sizeof(buf2));

		// Get confirmation from server
		n = recv(sock, buf2, sizeof(buf2), 0);
		if (n < 0)
		{
			printf("Could not confirmation from server...\n");
		}
		memset((void *)buf2, 0, sizeof(buf2));
	}
	// Clean up sockets
	WSACleanup();
}