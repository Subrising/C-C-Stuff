// shmemClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Constant Values
#define STDIN_FILENO 0

#define NB_ENABLE 1
#define NB_DISABLE 0

#define KEY 1080

// Shared Memory Struct
typedef struct
{
	// Values
	long number;
	long rNumber;
	int rNumbers[32];
	long slots[10];
	long sNumber;
	struct timeval start[10];
	struct timeval end[10];

	// Checks
	int cHandle;
	char qFlag[10];
	int qProgress[10];
	int processing;
	int qProcess[10];
	int tProcess;

} Shmem;

// Query Struct
typedef struct
{
	// Number and rotated numbers to factorise
	long number;
	int rNumbers[32];

	// Slot number and status of query
	int slot;
	int flag;

	// Current query progress
	int progress;

	// Test mode
	int test;
} TQuery;

// Struct for keeping track of the queries
typedef struct
{
	int id;
	int number;
	int slot;
	struct timeval start;
} CQuery;

// Global Variables
int id = 0;
Shmem * s_data = NULL;
CQuery queries[10];

int qID = 1;
int progress_last = 0;
TCHAR szName[] = TEXT("Global\MyFileMappingObject");
HANDLE hMapFile;

int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}

void clear_all()
{
	s_data->cHandle = 2;

	CloseHandle(hMapFile);

	printf("\n\nYOU HAVE DOOMED US ALL!!!\n");

	exit(0);
}

double time_diff(struct timeval x, struct timeval y)
{
	double x_ms, y_ms, diff;

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
			int   c = ratio * width;

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

	// Set up shared memory
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

	s_data->number = 0;
	s_data->rNumber = 0;
	for (int i = 0; i < 32; i++)
		s_data->rNumbers[i] = 0;
	for (int i = 0; i < 10; i++)
	{
		s_data->slots[i] = 0;
		s_data->qFlag[i] = 0;
		s_data->qProgress[i] = 0;
		s_data->qProcess[i] = 0;
	}
	s_data->sNumber = 0;
	s_data->cHandle = 0;
	s_data->processing = 0;

	int id;
	int number;
	int slot;
	struct timeval start;
	for (int i = 0; i < 10; i++)
	{
		queries[i].id = 0;
		queries[i].number = 0;
		queries[i].slot = 0;
	}

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

	// Initialise last operation timeval
	gettimeofday(&lastOp, NULL);

	while (1)
	{
		// If user is entering a number
		if (_kbhit())
		{
			char buffer[32];
			long fNum;

			memset(buffer, 0, sizeof(buffer));

			// Do not run until user has completed input
			if (scanf("%s", buffer) > 0)
			{
				// If user has opted to exit
				if (!strcmp(buffer, "q") || !strcmp(buffer, "quit"))
				{
					printf("Exiting...\n");
					break;
				}

				fNum = atol(buffer);

				printf("Number entered = %ld\n", fNum);
				printf("cHandle = %d\n", s_data->cHandle);

				// If user has entered an invalid input
				if (fNum == 0 && strcmp(buffer, "0") != 0)
				{
					printf("Incorrect input. Please try again.\n");
				}
				else
				{
					printf("s_data->processing = %d\n", s_data->processing);
					if ((fNum > 0 && s_data->tProcess == 0) || (fNum == 0 && s_data->processing == 0))
					{
						// Check to see if server is ready to read
						if (s_data->cHandle == 0)
						{
							s_data->number = fNum;
							s_data->cHandle = 1;

							// Wait for server to read
							while (s_data->cHandle != 0) {}

							// Set local query data
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
		
		// Print factors found and update query progress
		for (int i = 0; i < 10; i++)
		{
			if (s_data->qFlag[i] == 1)
			{
				facFound = 1;
				printf("Query %d: Factor of %ld = %ld\n", i, s_data->rNumber, s_data->slots[i]);
				s_data->qFlag[i] = 0;
			}

			if (s_data->qProgress[i] >= 100)
			{
				// Find the query
				for (int j = 0; j < 10; j++)
				{
					if (queries[j].id != 0 && queries[j].slot == i)
					{
						displayProgress(queries, sizeof(queries) / sizeof(CQuery), 28);
						double queryTime = 0;
						gettimeofday(&s_data->end[i], NULL);
						printf("Query complete for %d.\n", queries[j].number);
						queryTime = time_diff(s_data->start[i], s_data->end[i]);
						printf("Time taken for query (in microseconds) = %lf\n", queryTime);

						printf("\n");

						// Reset the space used for that query
						s_data->qProgress[i] = 0;
						s_data->qFlag[i] = 0;
						queries[j].id = 0;

						s_data->qProcess[i] = 0;

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

		if (facFound)
		{
			gettimeofday(&lastOp, NULL);
			progress_last = 0;
		}

		// Get current time and calculate difference from last operation
		gettimeofday(&now, NULL);
		time_difference = time_diff(now, lastOp);

		// If no user or server input for 500ms
		if (time_difference > 500000)
		{
			if (progress_last)
			{
				// Move up and clear the line
				//printf("\033[F\033[J");
				fflush(stdout);
				printf("\r");
				//system("cls");

				// If the progress bars move onto a second line
				int count = 0;
				for (int i = 0; i < sizeof(queries) / sizeof(CQuery); i++)
				{
					if (queries[i].id != 0)
						count++;
				}
				if (count > 5)
				{
					fflush(stdout);
					printf("\r");
					//printf("\033[F\033[J");
					//system("cls");
				}

			}
			// Display the progress bars
			displayProgress(queries, sizeof(queries) / sizeof(CQuery), 28);

			// Update the last operation time
			gettimeofday(&lastOp, NULL);
		}
		facFound = 0;
	}
}


int main(int argc, char * argv[])
{
	signal(SIGINT, clear_all);

	runClient(argc, argv);

	clear_all();

	return 0;
}
