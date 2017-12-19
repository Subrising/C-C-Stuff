#include "includes.h"

static char *myStrDup (char *str) 
{
    char *other = malloc (strlen (str) + 1);
    if (other != NULL)
        strcpy (other, str);
    return other;
}

void lsPipes(char * buf, int sendSocket)
{
    // Create file descriptors to use with dup
    int pipes[2];
    int stdin_cpy = dup(0);
    int stdout_cpy = dup(1);
    int stderr_cpy = dup(2);

    // Set up command checks
    int lfcheck = 0;
    int forcecheck = 0;
    int longcheck = 0;

    char * sendfile;

    printf("What is buf? It is '%s'", buf);
    char strbuf[1024];
    strcpy(strbuf, buf);

    // Check if command contains a .txt meaning we are writing output to a file
    if (strstr(strbuf, "txt") > 0)
    {
    	lfcheck = 1;
    	printf("LFCHECK SET\n");
    }
    strcpy(strbuf, buf);

    // Check if command contains a force overwrite
    if (strstr(strbuf, "-f") > 0)
    {
    	forcecheck = 1;
    	printf("FORCECHECK SET\n");
    }
    strcpy(strbuf, buf);

    // Check if user wants long listed ls
    if (strstr(strbuf, "-l") > 0)
    {
    	longcheck = 1;
    	printf("LONGCHECK SET\n");
    }
    strcpy(strbuf, buf);

    if (pipe(pipes) == -1) 
    {
        perror("Pipe failed");
        exit(1);
    }

    char *argv[100];
   	int argc = 0;

   	char *str = strtok(buf, " ");
    
   	while (str != NULL) 
   	{
       	argv[argc++] = myStrDup (str);
       	str = strtok (NULL, " ");
    }

    printf("What is argc up to? %d\n", argc);

    argv[argc] = 0;

    // Checking input arguments
    for (int i = 0; i <= argc; i++)
    {
       	printf ("Arg #%d = '%s'\n", i, argv[i]);
    }
    putchar ('\n');

    printf("Length of last string = %d\n", strlen(argv[argc-1]));
    printf("Total number of arguments = %d\n", argc);

    // Child process which will run ls command using execvp
    if (fork() == 0)
    {
    	close(pipes[0]);

    	dup2(pipes[1], 1);
    	dup2(pipes[1], 2);

    	close(pipes[1]);
    	char * prog1[5] = { 0 };

        // These if statements check the set commands and sets an appropriate argument list to run with execvp
    	if (argc == 1 || (argc == 2 && lfcheck == 1))
    	{
    		prog1[0] = "ls";
    		prog1[1] = 0;
    	}
    	else if (argc == 2 && longcheck == 1)
    	{
    		prog1[0] = "ls";
    		prog1[1] = "-l";
    		prog1[2] = 0;
    	}
    	else if (argc == 2)
    	{
    		prog1[0] = "ls";
    		prog1[1] = argv[1];
    		prog1[2] = 0;
    	}
    	else if (argc == 3 && longcheck == 1)
    	{
    		prog1[0] = "ls";
    		prog1[1] = "-l";
    		prog1[2] = argv[2];
    		prog1[3] = 0;
    	}
    	else if (argc == 3 && forcecheck == 1 && lfcheck == 1)
    	{
    		prog1[0] = "ls";
    		prog1[1] = 0;  
    	}
    	else if (argc == 3)
    	{
    		prog1[0] = "ls";
    		prog1[1] = argv[1];
    		prog1[2] = 0;
    	}
    	else if (argc == 4 && longcheck == 1)
    	{
    		prog1[0] = "ls";
    		prog1[1] = "-l";
    		prog1[2] = argv[2];
    		prog1[3] = 0;
    	}
    	else if (argc == 4 && forcecheck == 1)
    	{
    		prog1[0] = "ls";
    		prog1[1] = argv[2];
    		prog1[2] = 0;
    	}
    	else if (argc == 5)
    	{
    		prog1[0] = "ls";
    		prog1[1] = "-l";
    		prog1[2] = argv[3];
    		prog1[3] = 0;
    	}

        execvp(prog1[0], prog1);
        perror("execvp of ls failed");

        prog1[0] = NULL;

        exit(1);
    }
    else
    {
        // In parent to write to temporary ls file to read from later
    	char lsbuf[1024];

    	close(pipes[1]);

    	int fd = open("lstemp.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        // Set output to temporary file
    	dup2(fd, 1);
    	dup2(fd, 2);

        // Print to temporary file
    	while (read(pipes[0], lsbuf, sizeof(lsbuf)) != 0)
    	{
    		printf("%s", lsbuf);
    		bzero((void *)lsbuf, sizeof(lsbuf));
    	}

    	close(pipes[0]);
        close(pipes[1]);

        wait(0);

        // Reset STDIO to normal
  		dup2(stdin_cpy, 0);
  		dup2(stdout_cpy, 1);
  		dup2(stderr_cpy, 2);

  		close(fd);
  	}

    // Setting up file contents to send to client
	char line [1000];
	char buf2[1024];
	int flines;
	int linecount = 1;
	int n;
	char sendline[256];

	for (int i = 0; i < argc; i++)
	{
		char temp[128];
		strcpy(temp, argv[i]);
		printf("What is argv[%d]? It is '%s'\n", i, argv[i]);
		if (strstr(temp, "txt") > 0)
		{
			sendfile = argv[i];
			printf("Sendfile set to '%s'\n", argv[i]);
			break;
		}
		else 
			sendfile = NULL;
	}

	if (sendfile != NULL)
	{
		printf("Check...\n");
		printf("Sendfile = %s\n", sendfile);
	}

    // Telling client that we are sending file contents and that you should print the results to terminal
	if (sendfile == NULL)
	{
		printf("Sendfile is null\n");
		n = send(sendSocket, "recvline ", 9, 0);
		printf("Send recvline message...\n");
	}
    // Telling client that we are sending file contents and that you are to store them in sendfile
	else
	{
		printf("Sendfile is not null\n");
		sprintf(sendline, "recv ");
		strcat(sendline, sendfile);
		if (forcecheck == 1)
			strcat(sendline, " -f");
		n = send(sendSocket, sendline, sizeof(sendline), 0);
		printf("Send recv message...\n");
	}

	if (n < 0)
	{
		perror("Recvline failed...\n");
		return;
	}

	bzero((void *)buf2, sizeof(buf2));
	n = recv(sendSocket, buf2, sizeof(buf2), 0);
	if (n < 0)
	{
		perror("Did not get recvline successful message from client...\n");
		return;
	}
	printf("%s\n", buf2);

    // Open temporary file to read from
	FILE * fp;
	fp = fopen("lstemp.txt", "r");
	if (fp == NULL)
	{
		printf("Failed to open lstemp.txt\n");
	}

	n = send(sendSocket, "gtg", 3, 0);

    // Sending file contents to client
    while (fgets(line, sizeof(line), fp) != NULL)
	{
		printf("Line sent = %s", line);

		n = send(sendSocket, line, strlen(line), 0);
		if (n < 0)
		{
			perror("Message sending failed...\n");
			break;
		}

		bzero((void *)buf2, sizeof(buf2));
		n = recv(sendSocket, buf2, sizeof(buf2), 0);
		if (n < 0)
		{
			printf("Client did not respond...\n");
			break;
		}
		else
			printf("Received message = %s\n", buf);
	}

	printf("Finished sending contents...\n");

    n = send(sendSocket, "done", 4, 0);
	fclose(fp);

	close(sendSocket);

	fclose(fp);
}