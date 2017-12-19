#include "includes.h"

static char *myStrDup (char *str) 
{
    char *other = malloc (strlen (str) + 1);
    if (other != NULL)
        strcpy (other, str);
    return other;
}

int make_server_socket()
{
	int serverSock;
	struct sockaddr_in server;
	char buff[1024];
	int rval;

	// Create the socket
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock < 0)
	{
		perror("Socket Creation Failed...\n");
		exit(1);
	}
	else
	{
		printf("Socket Created...\n");

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(8080);

		int len = sizeof(server);
	
	// Bind the socket
		if (bind(serverSock, (struct sockaddr *) &server, sizeof(server)))
		{
			perror("Bind Failed...\n");
			exit(1);
		}

	// Start listening for connections on bound address
		printf("\tStarting Listener...\n");
		if (listen(serverSock, 5) != 0)
		{
			printf("\t\tFailed to start listener.\n");
			exit(1);
		}
		printf("\t\tSuccess.\n");

		printf("Socket successfully initialised.\n\n");
	}
	return serverSock;
}