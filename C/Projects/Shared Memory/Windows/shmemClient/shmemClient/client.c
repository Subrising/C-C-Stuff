#include "stdafx.h"

/*#include "client.h"

void clear_all()
{
	s_data->cHandle = 2;

	CloseHandle(hMapFile);

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


void runClient(int argc, char * argv[])
{
	printf("Greetings Child...\n");
	printf("I hear you would like to factorise a number?\n");
	printf("Well that will come at a price...\n\n");

	printf("Enter a number to factorise below, but be prepared... You may not get exactly what you wished for.\n");
	printf("(Enter quit or q to exit at your own peril...)\n");

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS, 
		FALSE,
		szName);

	if (hMapFile == NULL)
	{
		printf("Could not open file mapping object (%d).\n",
			GetLastError());
		return 1;
	}

	s_data = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		sizeof(s_data));

	if (s_data == NULL)
	{
		printf("Could not map view of file (%d).\n",
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}

	// Variables
	struct timeval now, lastOp;
	double time_difference = 0;

	//nonblock(NB_ENABLE);

	// Initialise last operation timeval
	gettimeofday(&lastOp, NULL);

	while (1)
	{
		if (kbhit())
		{
			char buffer[20];
			int fNum;

			//memset(buffer, 0, sizeof(buffer));

			bzero(buffer, sizeof(buffer));

			if (scanf("%s", buffer) > 0)
			{
				if (!strcmp(buffer, "q") || !strcmp(buffer, "quit"))
				{
					printf("Exiting...\n");
					break;
				}

				fNum = atoi(buffer);

				printf("Number entered = %d\n", fNum);
				printf("cHandle = %d\n", s_data->cHandle);

				if (fNum == 0 && strcmp(buffer, "0") != 0)
				{
					printf("Incorrect input. Please try again.\n");
				}
				else
				{
					printf("s_data->processing = %d\n", s_data->processing);
					if (fNum > 0 || (fNum == 0 && s_data->processing == 0))
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
				gettimeofday(&lastOp, NULL);
				progress_last = 0;
			}
		}

		int facFound;
		for (int i = 0; i < 10; i++)
		{
			if (s_data->qFlag[i] == 1)
			{
				facFound = 1;
				printf("Query %d: Factor of %d = %d\n", i, s_data->rNumber, s_data->slots[i]);
				s_data->qFlag[i] = 0;
			}

			if (s_data->qProgress[i] >= 100)
			{
				// Find the query
				for (int j = 0; j < 10; j++)
				{
					if (queries[j].id != 0 && queries[j].slot == i)
					{
						displayProgress(queries, sizeof(queries)/sizeof(CQuery), 28);
						double queryTime = 0;
						gettimeofday(&s_data->end[i], NULL);
						printf("Query complete for %d.\n", queries[j].number);
						queryTime = time_diff(s_data->start[i], s_data->end[i]);
						printf("Time taken for query (in microseconds) = %lf\n", queryTime);
						//s_data->start[i] = NULL;
						//s_data->end[i] = NULL;


						printf("\n");

						// Reset the space used for that query
						s_data->qProgress[i] = 0;
						s_data->qFlag[i] = 0;
						queries[j].id = 0;

						s_data->qProcess[i] = 0;

						for (int z = 0; z < 10; z++)
						{
							//printf("qProcess for slot %d = %d\n", z, s_data->qProcess[z]);
							if (s_data->qProcess[z] > 0)
							{
								s_data->processing = 1;
								//printf("s_data->processing = %d\n", s_data->processing);

								break;
							}
							else 
								s_data->processing = 0;
						}
					}
				}
			}
		}

		if (facFound)
		{
			gettimeofday(&lastOp, NULL);
			progress_last = 0;
		}

		// Get current time and calculate difference from last operation
		gettimeofday(&now, NULL);
		time_difference = time_diff(now, lastOp); // in microseconds
		// If 500ms has elapsed since server response or user input
		if (time_difference > 500000)
		{
			if (progress_last)
			{
				// Move up and clear the line
				printf("\033[F\033[J");

				// If the progress bars move onto a second line
				int count = 0;
				for (int i = 0; i < sizeof(queries)/sizeof(CQuery); i++)
				{
					if (queries[i].id != 0) 
						count++;
				}
				if (count > 5) 
					printf("\033[F\033[J");
			
			}
			// Display the progress bars
			displayProgress(queries, sizeof(queries)/sizeof(CQuery), 28);

			// Update the last operation time
			gettimeofday(&lastOp, NULL);
		}
		facFound = 0;
	}
}

*/