#include "includes.h"

static char *myStrDup (char *str) 
{
    char *other = malloc (strlen (str) + 1);
    if (other != NULL)
        strcpy (other, str);
    return other;
}

void reap(int sig)
{
	while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}

int main(int argc, char * argv[])
{
	pid_t pid;

	// Handle SIGCHLD by calling reap
	struct sigaction sigchld_action;
	memset(&sigchld_action, 0, sizeof (sigchld_action));
	sigchld_action.sa_handler = &reap;
	sigaction(SIGCHLD, &sigchld_action, NULL);

	printf("Starting the server...\n\n");

	// Start the server
	int sock = make_server_socket();
	if (sock < 0) 
	{
		perror("Socket failed...");
		exit(1);
	}

	printf("Server started.\n\n");
	printf("Waiting for incoming connections...\n");
	
	// Accept client connections to server
	while(1)
	{
		int csock = accept(sock, NULL, NULL);
		if (csock < 0)
		{
			continue;
		}
		
		// Create new process to handle client connection
		if((pid = fork()) == 0)
		{
			close(sock);

			process_request(csock);

			exit(0);
		}

		close(csock);
	}
	
	close(sock);
	printf("Server stopped.\n");

	return 0;
}