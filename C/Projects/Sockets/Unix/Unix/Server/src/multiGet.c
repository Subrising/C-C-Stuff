#include "includes.h"

static char *myStrDup (char *str) 
{
    char *other = malloc (strlen (str) + 1);
    if (other != NULL)
        strcpy (other, str);
    return other;
}

// Get | Grep Function
void multiGet(char * buf, int sendSocket)
{
	char *argv[100];
   	int argc = 0;

    char *str = strtok(buf, " ");
    
   	while (str != NULL) 
   	{
       	argv[argc++] = myStrDup (str);
       	str = strtok (NULL, " ");
    }

    // Child process for grep function
    if (fork() == 0) 
    {
        int putexists = 0;

        // Check if file exists
        struct stat st2;
		    putexists = stat(argv[1], &st2);
		    printf("Does this file exist? %s\n", (putexists == 0 ? "Yes" : "No"));

		    // Set up argument list for execvp
        char * sortedArgs[] = {"grep", argv[4], argv[1], 0};

       	printf("argv[1] = %s\n", argv[1]);
        printf("argv[4] = %s\n", argv[4]);

        // Set output to client
        dup2(sendSocket, STDOUT_FILENO);
        dup2(sendSocket, STDERR_FILENO);

        execvp(sortedArgs[0], sortedArgs);

        perror("execvp of get failed");
        exit(1);
    }

    wait(0);

    close(sendSocket);
}