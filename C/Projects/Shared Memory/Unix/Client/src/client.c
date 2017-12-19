#include "client.h"

void clear_all()
{
	s_data->cHandle = 2;

	if (shmdt(s_data) == -1)
		printf("Shared memory could not be detached...\n");

	printf("\n\nYOU HAVE DOOMED US ALL!!!\n");

	exit(0);
}

double time_diff(struct timeval x, struct timeval y)
{
	double x_ms , y_ms , diff;
	
	x_ms = (double)x.tv_sec * 1000000 + (double)x.tv_usec;
	y_ms = (double)y.tv_sec * 1000000 + (double)y.tv_usec;
	
	diff = (double)y_ms - (double)x_ms;

	return abs(diff);
}

void displayProgress(CQuery x[], int len, int width)
{
	int i, q = 0, n = 100;
	for (i = 0; i < len; i++)
	{
		// For each current query (ID not 0)
		if (x[i].id != 0)
		{
			// Calculuate the ratio of complete-to-incomplete.
			float ratio = s_data->qProgress[x[i].slot] / (float)n;
			int   c     = ratio * width;

			// Show the percentage complete.
			printf("Q%d: %3d%% [", x[i].id, (int)(ratio * 100));

			// Show the load bar.
			int j;
			for (j = 0; j < c; j++)
			   printf("=");

			for (j = c; j < width; j++)
			   printf(" ");

			printf("] ");

			q++;
		}
	}

	// If there was at least one query
	if (q)
	{
		printf("\n");

		// Set that the progress bar was the last thing in the console
		progress_last = 1;
	}
}

int kbhit()
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	fd_set fds;
	FD_ZERO(&fds);

	FD_SET(STDIN_FILENO, &fds); 
	select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

	return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state)
{
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);  //- get the terminal state

    if (state == NB_ENABLE)
    {
        ttystate.c_lflag &= ~ICANON;   //- turn off canonical mode
        ttystate.c_cc[VMIN] = 1;       //- minimum input chars read
    }
    else if (state==NB_DISABLE)
    {
       ttystate.c_lflag |= ICANON;     //- turn on canonical mode
    }
    //- set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void runClient(int argc, char * argv[])
{
	printf("Greetings Child...\n");
	printf("I hear you would like to factorise a number?\n");
	printf("Well that will come at a price...\n\n");

	printf("Enter a number to factorise below, but be prepared... You may not get exactly what you wished for.\n");
	printf("(Enter quit or q to exit at your own peril...)\n");

	// Getting shared memory, and setting it to s_data struct
	if ((id = shmget(KEY, sizeof(Shmem), 0666)) < 0)
	{
		printf("Could not get shared memory. Shared Memory Error = %d\n", errno);
		exit(1);
	} 

	printf("Shared Memory Received!\nSHMEM ID = %d\n", id);

	s_data = (Shmem *)shmat(id, 0, 0);

	if (s_data == (void *) -1)
	{
		printf("Could not get address of shared memory. Shared Memory Error = %d\n", errno);
		exit(1);
	}

	// Variables
	struct timeval now, lastQuery;
	double time_difference = 0;

	nonblock(NB_ENABLE);

	// Initialise last operation
	gettimeofday(&lastQuery, NULL);

	while (1)
	{
		// If user is entering a number
		if (kbhit())
		{
			char buffer[32];
			long fNum;

			bzero(buffer, sizeof(buffer));

			// Do not run until user has finished entering number
			if (scanf("%s", buffer) > 0)
			{
				// User has elected to quit
				if (!strcmp(buffer, "q") || !strcmp(buffer, "quit"))
				{
					printf("Exiting...\n");
					break;
				}

				fNum = atol(buffer);

				printf("Number entered = %d\n", fNum);
				printf("cHandle = %d\n", s_data->cHandle);

				// User input incorrect data
				if (fNum == 0 && strcmp(buffer, "0") != 0)
				{
					printf("Incorrect input. Please try again.\n");
				}
				else
				{
					printf("s_data->processing = %d\n", s_data->processing);

					// Checks to see if user input a number greater than 0, or if they input 0, there is no other query currently processing
					if ((fNum > 0 && s_data->tProcess == 0) || (fNum == 0 && s_data->processing == 0))
					{
						printf("cHandle = %d\n", s_data->cHandle);
						if (s_data->cHandle == 0)
						{
							s_data->number = fNum;
							s_data->cHandle = 1;

							while (s_data->cHandle != 0) {}

							for (int i = 0; i < sizeof(CQuery); i++)
							{
								if (queries[i].id == 0)
								{
									queries[i].id = qID;
									queries[i].number = fNum;
									queries[i].slot = s_data->sNumber;
									qID++;
									break;
								}
							}
						}
						else 
							printf("Server cannot process your query right now...\n Please wait...\n");
					}
					else 
						printf("Server cannot process your query right now...\n Please wait...\n");
				}
				gettimeofday(&lastQuery, NULL);
				progress_last = 0;
			}
		}

		int facFound;
		for (int i = 0; i < 10; i++)
		{
			// Check if there is data to be read from a slot
			if (s_data->qFlag[i] == 1)
			{
				facFound = 1;
				printf("Query %d: Factor of %ld = %ld\n", i, s_data->rNumber, s_data->slots[i]);
				s_data->qFlag[i] = 0;
			}

			// If a query has finished
			if (s_data->qProgress[i] >= 100)
			{
				// Find which query has finished
				for (int j = 0; j < 10; j++)
				{
					if (queries[j].id != 0 && queries[j].slot == i)
					{	
						// Print finished output
						displayProgress(queries, sizeof(queries)/sizeof(CQuery), 28);
						printf("Query complete for %d.\n", queries[j].number);

						// Get time taken
						double queryTime = 0;
						gettimeofday(&s_data->end[i], NULL);
						queryTime = time_diff(s_data->start[i], s_data->end[i]);
						printf("Time taken for query (in microseconds) = %lf\n", queryTime);

						printf("\n");

						// Reset the space used for that query
						s_data->qProgress[i] = 0;
						s_data->qFlag[i] = 0;
						queries[j].id = 0;

						s_data->qProcess[i] = 0;

						// Check to see if other queries are still processing
						for (int z = 0; z < 10; z++)
						{
							if (s_data->qProcess[z] > 0)
							{
								s_data->processing = 1;
								break;
							}
							else 
								s_data->processing = 0;
						}
					}
				}
			}
		}

		// Set that last printed output was not a progress bar and set time of last operation
		if (facFound)
		{
			gettimeofday(&lastQuery, NULL);
			progress_last = 0;
		}

		// Get current time and calculate difference from last operation
		gettimeofday(&now, NULL);
		time_difference = time_diff(now, lastQuery);
		
		// Check if no input from user or system for 500ms
		if (time_difference > 500000)
		{
			if (progress_last)
			{
				// Move up and clear the line
				printf("\033[F\033[J");

				// If progress bars go over 1 line
				int count = 0;
				for (int i = 0; i < sizeof(queries)/sizeof(CQuery); i++)
				{
					if (queries[i].id != 0) 
						count++;
				}
				if (count > 5) 
					printf("\033[F\033[J");
			
			}

			// Display progress bars of queries being run
			displayProgress(queries, sizeof(queries)/sizeof(CQuery), 28);

			// Update time of last operation
			gettimeofday(&lastQuery, NULL);
		}

		// Reset factor found
		facFound = 0;
	}
}
