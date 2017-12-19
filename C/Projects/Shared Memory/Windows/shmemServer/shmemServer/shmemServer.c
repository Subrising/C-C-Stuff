// shmemServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Constant Values
#define KEY 1080

// Semaphore Stuff
typedef struct
{
	pthread_mutex_t * mutex;
	pthread_cond_t * cd;
	int value;
} Semaphore;

Semaphore * s_get(int value)
{
	Semaphore * s = (Semaphore *)malloc(sizeof(Semaphore));

	s->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(s->mutex, NULL);

	s->cd = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	pthread_cond_init(s->cd, NULL);

	s->value = value;

	return s;
}

void s_wait(Semaphore * s)
{
	pthread_mutex_lock(s->mutex);

	while (s->value <= 0)
	{
		pthread_cond_wait(s->cd, s->mutex);
	}

	s->value--;

	pthread_mutex_unlock(s->mutex);
}

void s_signal(Semaphore * s)
{
	pthread_mutex_lock(s->mutex);

	s->value++;

	pthread_cond_signal(s->cd);
	pthread_mutex_unlock(s->mutex);
}

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
	// Initial number and rotated numbers to factorise
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

// Server Global Variables
int id = 0;                 // ID of Shared Memory Struct
Shmem * s_data = NULL;      // Shared Memory Struct
int testProgress = 0;
int testProgress2 = 0;
int qThreads = 0;

//TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
HANDLE hMapFile;


int threadCount = 320;		// Maximum thread count of 320. 32 Threads for each potential query, with maximum of 10 queries
pthread_t * threads = NULL; // Threads pointer

TQuery * queries = NULL;    // Queries Struct
Semaphore * q_sem = NULL;   // Semaphore for list of queries
Semaphore * s_slot[10];  	// Semaphore check for maximum number of queries

							// Function headers
void runServer(int argc, char * argv[]);
void * process_threads(void * pholder);
void clear_all();

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


int main(int argc, char * argv[])
{
	signal(SIGINT, clear_all);

	runServer(argc, argv);

	clear_all();

	return 0;
}

void clear_all()
{
	CloseHandle(hMapFile);

	printf("\n\nYOU HAVE DOOMED US ALL!!!\n");

	exit(0);
}

void * process_threads(void * pholder)
{
	while (1)
	{
		// Get a job
		s_wait(q_sem);

		int i, threadJob, slot, qFound;
		for (i = 0; i < threadCount; i++)
		{
			if (queries[i].flag == 1)
			{
				queries[i].flag = 2;
				threadJob = i;
				slot = queries[i].slot;
				qFound = 1;

				break;
			}
		}

		// If we found a job
		if (qFound)
		{
			printf("We found a job!\n");
			if (queries[threadJob].test == 1)
			{
				for (i = queries[threadJob].number; i < queries[threadJob].number + 10; i++)
				{
					// Wait while the slot has to be read by client
					s_wait(s_slot[slot]);
					//usleep(500000);

					// Put number to client
					s_data->rNumber = 0;
					s_data->slots[slot] = i;
					s_data->qFlag[slot] = 1;

					while (s_data->qFlag[slot] != 0) {}

					s_signal(s_slot[slot]);

					testProgress += 1;
					int checkProgress = ((testProgress / 30.0) * 100.0);
					printf("Test Progress = %d / 30\n%% Complete = %d\n", testProgress, checkProgress);

					queries[threadJob].progress = checkProgress;
				}
			}
			else
			{
				if (queries[threadJob].number <= 0)
				{
					queries[threadJob].progress = 100;
				}
				else
				{
					// For all the number between 1 and the given number
					for (long z = (long) 1; z <= queries[threadJob].number; z++)
					{
						// If i is a factor of the given number
						if (queries[threadJob].number % z == 0)
						{
							//printf("Found a factor for %ld, it is %ld\n", queries[threadJob].number, z);
							// Wait while the slot has to be read by client
							s_wait(s_slot[slot]);

							// Put number to client
							s_data->rNumber = queries[threadJob].number;
							s_data->slots[slot] = z;
							s_data->qFlag[slot] = 1;

							// Wait for the client to read the number
							while (s_data->qFlag[slot] != 0) {}

							// Signal another thread
							s_signal(s_slot[slot]);
							//printf("Another thread has been signalled...\n");
						}

						// Update progress of thread
						if (labs(z) == queries[threadJob].number)
						{
							queries[threadJob].progress = 100;
							printf("Eyy 100%% for %ld yo\n", queries[threadJob].number);
						}
						else
						{
							queries[threadJob].progress = ceil((int)(((float)labs(z) / labs(queries[threadJob].number) * 100.0)));
						}
					}
				}
			}
		}

		// Thread has finished the job
		queries[threadJob].flag = 3;
		printf("All Factors for %d found.\n", queries[threadJob].number);
	}
}

void runServer(int argc, char * argv[])
{
	printf("\t\tFactorise Server Engaged\n\n");
	printf("\t\tReady to perform some dope factorisations.\n\n");
	printf("\t\tResults will be sent to client\n\n");

	// Set up semaphores
	q_sem = s_get(0);

	for (int i = 0; i < 10; i++)
	{
		s_slot[i] = s_get(1);
	}

	if (argc == 2 && atoi(argv[1]) > 0)
	{
		threadCount = atoi(argv[1]) * 10;
	}

	// Set up queries
	printf("Thread Pool Size = %d\nThreads Per Query = %d\n", threadCount, threadCount / 10);

	queries = (TQuery *)malloc(sizeof(TQuery) * threadCount);
	for (int i = 0; i < threadCount; i++)
	{
		queries[i].flag = 0;
		queries[i].number = 0;
		int rNumbers[32];

		// Slot number and status of query
		queries[i].slot = 0;

		// Current query progress
		queries[i].progress = 0;

		// Test mode
		queries[i].test = 0;

		for (int j = 0; j < 32; j++)
		{
			queries[i].rNumbers[j] = 0;
		}
	}

	// Set up shared memory
	TCHAR szName[] = TEXT("Global\MyFileMappingObject");

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,
		4096,
		szName);                 // name of mapping object

	printf("File Map Created...\n");

	if (hMapFile == NULL)
	{
		printf("Could not open file mapping object (%d).\n",
			GetLastError());
		exit(1);
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

		exit(1);
	}

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

	printf("File Map Object Set...\n");

	// Set up threads
	threads = (pthread_t *)malloc(sizeof(pthread_t) * threadCount);

	for (int i = 0; i < threadCount; i++)
		pthread_create(&threads[i], NULL, process_threads, NULL);

	printf("Threads Created...\n");

	while (1)
	{
		// If conditional handle is invalid
		if (s_data->cHandle != 0 && s_data->cHandle != 1)
		{
			printf("Error receiving client handle information. Exiting...");
			break;
		}

		// If conditional handle shows there is a query to read
		if (s_data->cHandle == 1)
		{
			for (int i = 0; i < 10; i++)
			{
				// Find first empty query slot
				if (s_data->slots[i] == 0 && s_data->qFlag[i] != 1)
				{
					long facNumber = s_data->number;

					s_data->processing = 1;
					s_data->qProcess[i] = 1;

					s_data->sNumber = i;
					s_data->cHandle = 0;

					printf("Adding query for number %d, to slot %d\n", facNumber, i);

					if (facNumber == 0)
						qThreads = 3;
					else
						qThreads = threadCount / 10;

					printf("qThreads = %d\n", qThreads);

					// Go through threadcount and find a query that is empty to start the factorisation
					for (int x = 0; x < qThreads; x++)
					{
						printf("x = %d\n", x);
						
						for (int z = 0; z < threadCount; z++)
						{
							printf("queries[%d].flag = %d\n", z, queries[z].flag);
							if (queries[z].flag == 0 || queries[z].flag == NULL)
							{
								queries[z].slot = i;

								// Test run
								if (facNumber == 0)
								{
									printf("Starting test run...\n");
									gettimeofday(&s_data->start[i], NULL);
									s_data->tProcess = 1;
									queries[z].test = 1;
									queries[z].number = x * 10;
								}
								// Factorisation
								else
								{
									if (x == 0)
									{
										printf("First number to factorise...\n");
										gettimeofday(&s_data->start[i], NULL);
										queries[z].number = labs(facNumber);
									}
									else
									{
										printf("%ld number to factorise...\n", x);
										long factorQ = (facNumber >> x) | (facNumber << qThreads - x);
										queries[z].number = labs(factorQ);
									}
								}

								queries[z].flag = 1;
								queries[z].progress = 0;

								// Signal thread to wake and factorise number
								s_signal(q_sem);

								break;
							}
						}
					}
					break;
				}
			}
		}

		// Update progress for each query
		for (int i = 0; i < 10; i++)
		{
			int threads = 0, percent = 0;
			for (int j = 0; j < threadCount; j++)
			{
				// If the job has been accepted by a thread
				if (queries[j].flag < 2)
					continue;

				// Sum the progress of jobs related to this slot
				if (queries[j].slot == i)
				{
					percent += queries[j].progress;
					threads++;
				}
			}

			// Factorisation update
			if (threads > 0 && qThreads != 3 && s_data->qProcess[i] == 1)
			{
				s_data->qProgress[i] = ceil((int)((float)percent / (float)qThreads));
			}
			// Test update
			else if (threads > 0 && qThreads == 3 && s_data->qProcess[i] == 1 && s_data->tProcess == 1)
			{
				s_data->qProgress[i] = ((testProgress / 30.0) * 100.0);
				if (s_data->qProgress[i] >= 100)
				{
					s_data->tProcess = 0;
					testProgress = 0;
				}
			}
		}
	}
}
