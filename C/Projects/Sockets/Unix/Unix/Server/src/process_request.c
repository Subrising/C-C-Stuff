#include "includes.h"

static char *myStrDup (char *str) 
{
    char *other = malloc (strlen (str) + 1);
    if (other != NULL)
        strcpy (other, str);
    return other;
}

int process_request(int socket)
{
	char buf[1024];
	char buf2[1024];
	char * temp;
	char cmd[20];
	char args [5][50];
	int argc;
	int n, i;

	bzero((void *)buf, sizeof(buf));
	bzero((void *)buf2, sizeof(buf2));
	bzero((void *)cmd, sizeof(cmd));
	bzero((void *)args, sizeof(args));

	// Read in the input from the client
	n = recv(socket, buf, sizeof(buf), 0);

	strcpy(buf2, buf);

	if (n <= 0)
	{
		printf("\tError receiving input from client or connection was forcefully closed!\n");
	}

	// Split the input from the client into command and arguments
	if ((temp = strtok(buf, " ")) != NULL)
		strcpy(cmd, temp);

	temp = strtok(NULL, " "); i = 0;
	printf("args[0] = %s\n", temp);
	while (temp != NULL && i < 5)
	{
		strcpy(args[i], temp); i++; argc++;
		temp = strtok(NULL, " ");
		printf("args[%d] = %s\n", i, temp);
	}

	// Checking if client command contained a pipe
	if ((strpbrk(buf2, "|")) > 0)
	{
		printf("Found Pipe. Amount of arguments = %d\n", argc);

		// Run ls | sort
		if (argc == 2 && !strcmp(cmd, "ls"))
		{
			printf("Running multiExec...\n");
			multiExec(buf2, socket);
		}
		// Run get | grep
		else if (argc == 4 && !(strcmp(cmd, "get")))
		{
			printf("RUnning multiGet...\n");
			multiGet(buf2, socket);
		}
		bzero((void *)buf2, sizeof(buf2));
	}
	// Get system information
	else if (!strcmp(cmd, "sys"))
	{
		struct utsname os;
		bzero((void *)buf, sizeof(buf));

		n = uname(&os);

		if (n < 0)
		{
			printf("\tError retrieving system info.\n");

			n = send(socket, "Could not retrieve system info.\n", 37, 0);
			if (n < 0)
				printf("\t\tError sending error message to client\n");
			else
				printf("\t\tSent error message to client\n");
		}
		else
		{
			strcpy(buf, "\n\tOS: ");
			strcat(buf, os.sysname);
			strcat(buf, "\n\tVersion: ");
			strcat(buf, os.version);
			strcat(buf, "\n\tCPU: ");
			strcat(buf, os.machine);
			strcat(buf, "\n");

			n = send(socket, buf, strlen(buf), 0);
			if (n < 0)
				printf("\tError sending system info to client\n");
			else
				printf("\tSent system info to client\n");
		}
	}
	// Complete delay command
	else if (!strcmp(cmd, "delay"))
	{
		if (argc != 1)
		{
			n = send(socket, "No time specified.\n", 20, 0);
			if (n < 0)
				printf("\tError sending error message to client\n");
			else
				printf("\tSent error message to client\n");

			return 0;
		}
		
		sleep(atoi(args[0]));

		bzero((void *)buf, sizeof(buf));
		
		strcpy(buf, args[0]);
		strcat(buf, "\n");

		n = send(socket, buf, strlen(buf), 0);
		if (n < 0)
			printf("\tCould not send delayed message to client.\n");
		else
			printf("\tSent client delayed message after %s seconds.\n", args[0]);
	}
	// Perform ls command
	else if (!strcmp(cmd, "ls"))
	{
		printf("Doing lsPipes...\n");
		lsPipes(buf2, socket);
		bzero((void *)buf2, sizeof(buf2));

		printf("ls finished...\n");

		return 0;
	}
	// Perform file put to server from client
	else if (!strcmp(cmd, "put"))
	{
		int putexists = 0;
		int putforce = 0;
		char * putread = NULL;
		char * putfile = NULL;

		// Grab arguments from given command
		printf("Put arguments = %d\n", argc);
		for (i = 0; i < argc; i++)
		{
			if (!strcmp(args[i], "-f")) 
			{
				putforce = 1;
			}
			else if (putread == NULL)
			{
				putread = args[i];
				printf("Putread is set to args[%d] which is %s\n", i, args[i]);
			}
			else if (putfile == NULL)
			{
				putfile = args[i];
				printf("Putfile is set to args[%d] which is %s\n", i, args[i]);
			}
		}

		// No file specified
		if (putread == NULL)
		{
			n = send(socket, "No file specified.\n", 20, 0);
			if (n < 0)
				printf("\tError sending error message to client.\n");
			else
				printf("\tSent error message to client.\n");

			return 0;
		}

		bzero((void *)buf, sizeof(buf));

		// Telling client to send file contents to server and whether or not we have set a force overwrite
		strcpy(buf, "sendfile ");

		char tempArgs[10];
		if (putforce == 1)
		{
			sprintf(tempArgs, "%d -f ", argc);
		}
		else
			sprintf(tempArgs, "%d ", argc);

		strcat(buf, tempArgs);
		strcat(buf, putread);
		n = send(socket, buf, sizeof(buf), 0);

		if ( n < 0 )
		{
			perror("Failed sending received argument...\n");
			return 0;
		}

		printf("Finishing sending sendfile name...\n");

		printf("We sent '%s'\n", buf);

		printf("File given: %s\n", putread);

		// If no specified file name was given to store file contents to, create a file with same name as file sent from client
		if (putfile == NULL)
		{
			// Check is file sent from client has an absolute path and take everything from after last "/"
			if (strpbrk(putread, "/") > 0)
			{
				int putlength = strlen(putread);
				printf("Putread length = %d\n", putlength);
				printf("putread before / check = %s\n", putread);
				const char putcheck = '/';
				char * sindex;

				sindex = strrchr(putread, putcheck);

				putfile = sindex + 1;
				printf("Checking putfile 1... Putfile = %s\n", putfile);
			}
			// Client sent relative path
			else
			{
				putfile = putread;

				printf("Checking putfile 2... Putfile = %s\n", putfile);
			}
		}

		printf("Putfile = %s\n", putfile);

		// Checks to see if file on server is a relative or absolute path
		if (putfile != NULL && (strpbrk(putfile, "c:") == 0))
		{
			char temp2[256];

			getcwd(temp2, sizeof(temp2));

			strcat(temp2, "/");

			strcat(temp2, putfile);

			putfile = temp2;
			printf("Relative path was given so absolute path was created.\nNew Path = %s\n", putfile);
		}

		printf("Renaming to: %s\n", putfile);
		printf("Forced overwrite: %s\n", (putforce ? "Yes" : "No"));

		printf("Checking putfile name 1... It is '%s'\n", putfile);

		char puttemp[256];
		strcpy(puttemp, putfile);


		// Check if the file exists
		struct stat st;
		putexists = stat(putfile, &st);
		printf("Checking putfile name 2... It is '%s'\n", putfile);
		if (putexists == 0 && !putforce)
		{
			printf("The file already exists and you have not specified force [-f].\n");

			n = send(socket, "0", 1, 0);
			if (n < 0)
			{
				perror("Exists message could not be sent...\n");
			}
		}
		else
		{
			printf("Checking putfile name 3... It is '%s'\n", putfile);
			n = send(socket, "1", 1, 0);
			if (n < 0)
			{
				perror("Writing message could not be sent...\n");
				close(socket);
				return 0;
			}

			printf("We are ready to write to file!\n");
			printf("Checking putfile name 4... It is '%s'\n", putfile);

			n = recv(socket, buf, sizeof(buf), 0);
			if (n < 0)
			{
				printf("Error receiving good to go message.\n");
			}
			else
			{
				printf("Did we get a gtg message? '%s'\n", buf);

				// Open the file to write to
				FILE * fp;
				printf("Checking putfile name 5... It is '%s'\n", putfile);
				fp = fopen(putfile, "w");

				// Receive file contents from client
				bzero((void *)buf, sizeof(buf));
				n = recv(socket, buf, sizeof(buf), 0);
				printf("First buffer result = %s\n", buf);
				n = fwrite(buf, sizeof(char), strlen(buf), fp);

				int count = 1;

				while (1)
				{								
					int scheck = send(socket, "received", 8, 0);
					if (scheck < 0)
					{
						perror("We're done here...\n");
						return 0;
					}

					bzero((void *)buf, sizeof(buf));

					n = recv(socket, buf, sizeof(buf), 0);
					printf("Buffer Result %d = %s\n", count, buf);

					if (strcmp(buf, "done") == 0)
					{
						break;
					}

					n = fwrite(buf, sizeof(char), strlen(buf), fp);
					if (n < 0)
						printf("Error writing to file.\n");
					count++;
				}

				fclose(fp);

				if(putforce && !putexists)
				{
					printf("Get successful, saved as %s. Original was overwritten.\n", putfile);
				}
				else
				{
					printf("Get successful, saved as %s.\n", putfile);
				}
			}
		}
	}
	// Perform get command
	else if (!strcmp(cmd, "get"))
	{
		int exists = 0;
		int force = 0;
		char * readfile = NULL;
		char * remotefile = NULL;

		// Grab arguments from given client command
		printf("Get arguments = %d\n", argc);
		for (i = 0; i < argc; i++)
		{
			if (!strcmp(args[i], "-f")) 
			{
				force = 1;
			}
			else if (readfile == NULL)
			{
				readfile = args[i];
				printf("Readfile is set to args[%d] which is %s\n", i, args[i]);
			}
			else if (remotefile == NULL)
			{
				remotefile = args[i];
				printf("Remotefile is set to args[%d] which is %s\n", i, args[i]);
			}
		}

		// Check to see if file requested was an absolute or relative path
		if (strpbrk(readfile, "c:") == 0)
		{
			char temp[128];
			getcwd(temp, sizeof(temp));
			strcat(temp, "/");
			strcat(temp, readfile);

			readfile = temp;
			printf("Relative path was given so absolute path was created.\nNew Path = %s\n", readfile);
		}

		// No file specified
		if (readfile == NULL)
		{
			n = send(socket, "No file specified.\n", 20, 0);
			if (n < 0)
				printf("\tError sending error message to client.\n");
			else
				printf("\tSent error message to client.\n");

			return 0;
		}

		printf("Requested file: %s\n", readfile);
		printf("Renamed to: %s\n", remotefile);
		printf("Forced overwrite: %s\n", (force ? "Yes" : "No"));

		// Check if the file exists
		struct stat filest;
		exists = stat(readfile, &filest);
		if (exists == 0)
		{
			printf("File Exists\n");
		}
		else
		{
			printf("File not found...\n");

			n = send(socket, "File not found.\n", 17, 0);
			if (n < 0)
				printf("Could not send error message to client...\n");
			else
				printf("Error message sent to client...\n");

			return 0;
		}

		if (remotefile != NULL)
		{
		// Tell client to see if the file they are storing to already exists
			bzero((void *)buf, sizeof(buf));

			strcpy(buf, "recv ");
			strcat(buf, (remotefile ? remotefile : readfile));
			if (force)
				strcat(buf, " -f");

			n = send(socket, buf, strlen(buf), 0);

			if (n < 0)
			{
				printf("\tError sending existing file check request.\n");

				return 0;
			}

			bzero((void *)buf, sizeof(buf));

			n = recv(socket, buf, sizeof(buf), 0);

			if (n < 0)
				printf("\tError receiving file from client.\n");

			printf("File exist check = %s\n", buf);
			
			if (!strcmp(buf, "0"))
			{
				printf("\tClient called for abort.\n");

				return 0;
			}
			
			// Open file to send contents from
			FILE * fp;
			unsigned long fsize;

			fp = fopen(readfile, "r");

			if (fp == NULL) 
			{
				printf("Could not open file!\n");

				n = send(socket, "File does not exist or could not be opened.\n", 45, 0);
				if (n < 0)
					printf("\tError sending error message to client.\n");
				else
					printf("\tSent error message to client.\n");

				return 0;
			}

        		char line [1000];
        		int flines;
        		int linecount = 1;
        		
        		bzero((void *)buf, sizeof(buf));
        		n = send(socket, "gtg", 3, 0);

        		// Send file contents to client
        		while(fgets(line, sizeof(line), fp) != NULL) 
        		{
            		printf("Line sent = %s", line);

					n = send(socket, line, strlen(line), 0);
					if (n < 0)
					{
        				perror("Message sending failed...\n");
        				break;
        			}

        			bzero((void *)buf, sizeof(buf));
        			n = recv(socket, buf, sizeof(buf), 0);
        			if (n < 0)
        			{
        				printf("Client did not respond...\n");
        				break;
        			}
        			else
        				printf("Received message = %s\n", buf);
        		}

        		printf("Finished sending contents...\n");

        		n = send(socket, "done", 4, 0);
				fclose(fp);


			fclose(fp);

			printf("\tFile transfer complete.\n");
		}
		// If no remote file has been specified for the client
		else
		{
			FILE * fp;
			unsigned long fsize;

			fp = fopen(readfile, "r");

			if (fp == NULL) 
			{
				printf("Could not open file!\n");

				n = send(socket, "File does not exist or could not be opened.\n", 45, 0);
				if (n < 0)
					printf("\tError sending error message to client.\n");
				else
					printf("\tSent error message to client.\n");

				return 0;
			}

			if (fp != NULL) 
    		{
        		char line [1000];
        		int flines;
        		int linecount = 1;

        		n = send(socket, "recvline ", 9, 0);
        		if (n < 0)
        		{
        			perror("Recvline failed...\n");
        			return 0;
        		}

        		bzero((void *)buf, sizeof(buf));
        		n = recv(socket, buf, sizeof(buf), 0);
        		if (n < 0)
        		{
        			perror("Did not get recvline successful message from client...\n");
        			return 0;
        		}
        		printf("%s\n", buf);

        		// Send file contents to client
        		while( fgets(line, sizeof(line), fp) != NULL)
        		{
            		printf("Line sent = %s", line);

					n = send(socket, line, strlen(line), 0);
					if (n < 0)
					{
        				perror("Message sending failed...\n");
        				break;
        			}

        			bzero((void *)buf, sizeof(buf));
        			n = recv(socket, buf, sizeof(buf), 0);
        			if (n < 0)
        			{
        				printf("Client did not respond...\n");
        				break;
        			}
        			else
        				printf("Received message = %s\n", buf);
        		}

        		printf("Finished sending contents...\n");

        		n = send(socket, "done", 4, 0);
				fclose(fp);
			}
		}
	}
	// Unknown command received
	else
	{
		n = send(socket, "Error: Unknown Command\n", 24, 0);
		if (n < 0)
		{
			printf("Error sending unknown command notification.\n");
		}
		printf("\tReceived an unknown command.\n");
	}

	return 0;
}