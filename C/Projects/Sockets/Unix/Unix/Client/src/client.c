#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

#include <string.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char * argv[])
{
	int connectionArr[10];
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) 
    {
       printf("usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);

    bzero((char *) &serv_addr, sizeof(serv_addr));

	// Get the host's address by their name
	server = gethostbyname(argv[1]);

    if (server == NULL) 
    {
        printf("ERROR, Cannot find host...\n");
        exit(0);
    }

	// Copy address into socket address, set family to AF_INET

	bcopy(server->h_addr, (struct sockaddr *) &serv_addr.sin_addr, server->h_length);

	serv_addr.sin_port = htons(portno);
	serv_addr.sin_family = AF_INET;

	printf("Address: %s\n", inet_ntoa(serv_addr.sin_addr));
	printf("Port: %d\n", portno);
	printf("Ready to connect to server!\n\n");

	// Set up file descriptor set
	int opt = 1;  
    int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;  
    int max_sd;  
    struct sockaddr_in address;  
       
    // Data buffer of 1K 
    char buffer[1024];  
        
    // Set of socket descriptors 
    fd_set master, readfds;    

    // Clear the socket set 
    FD_ZERO(&master);  
    
    //add master socket to set 
    FD_SET(STDIN_FILENO, &master);  
    max_sd = STDIN_FILENO;
    client_socket[0] = max_sd;  

    printf("Added STDIN_FILENO to FD SET as %d\n", max_sd);
        
    printf("> ");
	fflush(stdout);
	int currentfds = 1;

	for (int j = 1; j < max_clients; j++)
		client_socket[j] = 0;

	while(1)  
	{  
        // Wait for an activity on one of the sockets
		readfds = master;
		
		// Check for socket activity
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);  

		if (activity < 0)  
		{  
			printf("select error");
			exit(4);  
		}  

		for (i = 0; i < sizeof(master); i++)  
		{  
			sd = client_socket[i]; 

			if (FD_ISSET(sd, &readfds))
			{
				struct timeval start;


				if (sd == STDIN_FILENO)
				{
					gettimeofday(&start, NULL);

					// Read in user input and write to the server
					bzero((void *)&buffer, sizeof(buffer));
					scanf("%256[^\n]%*c", buffer);

					if(!strcmp(buffer, "quit"))
					{
						exit(0);
					}

					// Make connection to the server
					int sock = socket(AF_INET, SOCK_STREAM, 0);
					if(sock == -1)
					{
						perror("Error creating socket...\n");
						exit(1);
					}

					if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0)
					{
						printf("Failed to connect to server...\n");
						exit(1);
					}

					n = send(sock, buffer, strlen(buffer), 0);

					if (n < 0)
					{
						perror("Send Failed...");
						break;
					}

					FD_SET(sock, &master);
					currentfds++;
					for (int z = i; z < max_clients; z++)  
					{  
                		// If position is empty 
						if( client_socket[z] == 0 )  
						{  
							client_socket[z] = sock;  
							printf("Adding to list of sockets as %d\n" , client_socket[z]);
							if (sock > max_sd)
								max_sd = sock;  
							break;  
						}  
					}  

					printf("> ");
					fflush(stdout);
					break;
				}
				else
				{
					// Clear the line we will print on
					printf("\33[2K\r");

					// Read response from the server
					bzero((void *)&buffer, sizeof(buffer));

					n = recv(sd, buffer, sizeof(buffer), 0);

					if (n <= 0)
					{
						printf("Nothing received from server...\n");
					}

					else
					{
						char temp[256];
						strcpy(temp, buffer);

						char * command = NULL;
						command = strtok(temp, " ");

						//////////////////////////////////////////////////////////////
						// This section receives file contents sent from the server //
						// and prints it to a file                                  // 
						//////////////////////////////////////////////////////////////
						if (!strcmp(command, "recv"))
						{
							int size = 0;
							int force = 0;
							int exists = 0;
							char * filename = NULL;
							char * temp = NULL;
							filename = strtok(NULL, " ");

							// Create an absolute path for filename if a relative path was given
							if (strpbrk(filename, "c:") == 0)
							{
								char temp[128];
								getcwd(temp, sizeof(temp));
								strcat(temp, "/");
								strcat(temp, filename);

								filename = temp;
								printf("Relative path was given so absolute path was created.\nNew Path = %s\n", filename);
							}

							temp = strtok(NULL, " ");
							if (temp != NULL && !strcmp(temp, "-f"))
							{
								force = 1;
								printf("Force overwrite was set...\n");
							}

							char recvtemp[256];
							strcpy(recvtemp, filename);

							// Check if the file exists
							struct stat st;
							exists = stat(filename, &st);
							if (exists == 0 && !force)
							{
								printf("The file already exists and you have not specified force [-f].\n");

								n = send(sd, "0", 1, 0);
								if (n < 0)
								{
									perror("Exists Message could not be sent...\n");
								}
							}
							else
							{
								n = send(sd, "1", 1, 0);
								if (n < 0)
								{
									perror("Writing message could not be sent...\n");
									break;
								}

								printf("We are ready to write to file!\n");

								n = recv(sd, buffer, sizeof(buffer), 0);
								if (n < 0)
								{
									printf("Error receiving file size from server.\n");
								}
								else
								{
									// Open the file
									FILE * fp;
									fp = fopen(filename, "w");
									printf("filename = %s\n", filename);
									printf("Is fp = NULL? %s\n", fp != NULL ? "No" : "Yes");

									printf("Buffer = %s\n", buffer);

									// Get first line of file from server
									bzero((void *)buffer, sizeof(buffer));
									n = recv(sd, buffer, sizeof(buffer), 0);
									printf("Line 1: %s\n", buffer);

									// Write first line to file
									n = fwrite(buffer, sizeof(char), strlen(buffer), fp);

									int count = 1;

									// Continue receiving file contents from server to write to file
									while (1)
									{								
										int scheck = send(sd, "received", 8, 0);
										if (scheck < 0)
										{
											perror("We're done here...\n");
											return 0;
										}

										bzero((void *)&buffer, sizeof(buffer));

										n = recv(sd, buffer, sizeof(buffer), 0);
										printf("Buffer Result %d = %s\n", count, buffer);

										if (strcmp(buffer, "done") == 0)
										{
											break;
										}

										n = fwrite(buffer, sizeof(char), strlen(buffer), fp);
										if (n < 0)
											printf("Error writing to file.\n");
										count++;
									}

									fclose(fp);

									if(force && !exists)
									{
										printf("Get successful, saved as %s. Original was overwritten.\n", filename);
									}
									else
									{
										printf("Get successful, saved as %s.\n", filename);
									}
								}
							}
							// Print time taken to complete action
							struct timeval end;
							gettimeofday(&end, NULL);
							int milliseconds = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
							printf("\nTotal time taken is : %d milliseconds.\n", milliseconds);
						}
						//////////////////////////////////////////////////////////////
						// This section receives file contents sent from the server //
						// and prints it to the terminal                            // 
						//////////////////////////////////////////////////////////////
						else if (!strcmp(command, "recvline"))
						{
							printf("Recvline successful...\n");
							n = send(sd, "Received getline message", 24, 0);
							int countlines = 1;

							// Get first line of file from server
							bzero((void *)buffer, sizeof(buffer));
							n = recv(sd, buffer, sizeof(buffer), 0);
							printf("Line 0 = %s\n", buffer);
							
							int count = 1;
							while (1)
							{
								// Print 40 lines at a time, then stop and wait for user to enter another input to continue
								while (count % 40 != 0)
								{
									int scheck = send(sd, "received", 8, 0);
									if (scheck < 0)
									{
										perror("We're done here...\n");
										return 0;
									}

									bzero((void *)buffer, sizeof(buffer));

									n = recv(sd, buffer, sizeof(buffer), 0);
									printf("Line %d: %s\n", count, buffer);
									if (strcmp(buffer, "done") == 0)
									{
										break;
									}
									count++;
								}
								// If server sent message done, printing is complete
								if (strcmp(buffer, "done") == 0)
								{
									break;
								}
								printf("Need to wait...\n");
								getchar();
								count++;
							}
							// Print out the time taken to complete
							struct timeval end;
							gettimeofday(&end, NULL);
							int milliseconds = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
							printf("\nTotal time taken is : %d milliseconds.\n", milliseconds);
						}
						////////////////////////////////////////////////////
						// This section sends file contents to the server // 
						////////////////////////////////////////////////////
						else if (!strcmp(command, "sendfile"))
						{
							int exists = 0;
							int force = 0;
							int sargs = 0;
							char * sendfile = NULL;

							// Grab arguments
							char * temp = NULL;

							sargs = atoi(strtok(NULL, " "));

							printf("Amount of arguments = %d\n", sargs);

							// Checks the arguments received from server along with sendfile command
							if (sargs == 1)
							{
								sendfile = strtok(NULL, " ");
							}
							else if (sargs >= 2)
							{
								temp = strtok(NULL, " ");
								if (!strcmp(temp, "-f")) 
								{
									force = 1;
									sendfile = strtok(NULL, " ");
								}
								else
									sendfile = temp;
							}

							printf("Sendfile is set to %s\n", sendfile);

							printf("Force tokenise finished...\n");

							// If no file was specified
							if (sendfile == NULL)
							{
								n = send(sd, "No file specified.\n", 20, 0);
								if (n < 0)
									printf("\tError sending error message to client.\n");
								else
									printf("\tSent error message to client.\n");

								return 0;
							}

							// If a relative path was given, make it an absolute path to use
							if (strpbrk(sendfile, "c:") == 0)
							{
								char temp[128];
								getcwd(temp, sizeof(temp));
								strcat(temp, "/");
								strcat(temp, sendfile);

								sendfile = temp;
								printf("Relative path was given so absolute path was created.\nNew Path = %s\n", sendfile);
							}

							printf("Requested file: %s\n", sendfile);
							printf("Forced overwrite: %s\n", (force ? "Yes" : "No"));

							char temp2[256];
							strcpy(temp2, sendfile);

							// Check if the file already exists
							struct stat filest;
							exists = stat(sendfile, &filest);
							if (exists == 0)
							{
								printf("File Exists\n");
							}
							else
							{
								printf("File not found...\n");

								n = send(sd, "File not found.\n", 17, 0);
								if (n < 0)
									printf("Could not send error message to client...\n");
								else
									printf("Error message sent to client...\n");

								return 0;
							}

							// Check to see if sendfile exists, if so send its contents to server
							if (sendfile != NULL)
							{
								// Tell client to check for existing file first

								bzero((void *)buffer, sizeof(buffer));

								n = recv(sd, buffer, sizeof(buffer), 0);

								if (n < 0)
								{
									printf("\tError receiving check message from server.\n");
									close(sd);
									break;
								}

								printf("File exist check = %s\n", buffer);

								if (!strcmp(buffer, "0"))
								{
									printf("\tServer called for abort.\n");
									close(sd);
									break;
								}

								// Create file to write to
								FILE * fp;
								unsigned long fsize;

								fp = fopen(sendfile, "r");

								if (fp == NULL) 
								{
									printf("Could not open file!\n");

									n = send(sd, "File does not exist or could not be opened.\n", 45, 0);
									if (n < 0)
										printf("\tError sending error message to client.\n");
									else
										printf("\tSent error message to client.\n");

									return 0;
								}

								char line [1000];
								int flines;
								int linecount = 1;

								bzero((void *)buffer, sizeof(buffer));
								n = send(sd, "gtg", 3, 0);

								// Reads from file and sends to server line by line
        						while(fgets(line, sizeof(line), fp) != NULL) 
								{
									printf("Line sent = %s", line);

									n = send(sd, line, strlen(line), 0);
									if (n < 0)
									{
										perror("Message sending failed...\n");
										break;
									}

									bzero((void *)&buffer, sizeof(buffer));
									n = recv(sd, buffer, sizeof(buffer), 0);
									if (n < 0)
									{
										printf("Server did not respond...\n");
										break;
									}
									else
										printf("Received message = %s\n", buffer);
								}

								printf("Finished sending contents...\n");

								n = send(sd, "done", 4, 0);
								fclose(fp);


								fclose(fp);

								printf("\tFile transfer complete.\n");

								// Print out the time taken to complete
								struct timeval end;
								gettimeofday(&end, NULL);
								int milliseconds = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
								printf("\nTotal time taken is : %d milliseconds.\n", milliseconds);
							}
						}
						// This gets the response back from the server
						else
						{
							printf("Response: ");
							fprintf(stdout, buffer, n);

							// Print out the time taken to complete
							struct timeval end;
							gettimeofday(&end, NULL);
							int milliseconds = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
							printf("\nTotal time taken is : %d milliseconds.\n", milliseconds);
						}
					}

					close(sd);
					FD_CLR(sd, &master);

                    //Close the socket and mark as 0 in list for reuse  
					client_socket[i] = 0; 
					currentfds--; 

					printf("> ");
					fflush(stdout);
					break;
				}
			}
		}
	}   
	return 0;
}