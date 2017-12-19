#include "server.h"

// Detaches and deletes shared memory, then exits program
void clear_all()
{
	if (shmdt(s_data) == -1)
		printf("Shared memory could not be detached...\n");

	if (shmctl(id, IPC_RMID, NULL) == -1)
		printf("Could not mark shared memory for destruction...\n");

	printf("\n\nYOU HAVE DOOMED US ALL!!!\n");

	exit(0);
}

void * process_threads(void * pholder)
{
	while (1)
	{
		// Get a job
		s_wait(q_sem);

		int i, slot, threadQuery, qFound;
		for (i = 0; i < threadCount; i++)
		{
			// If thread has been flagged for a query
			if (queries[i].flag == 1)
			{
				// Set flags to show thread is processing a query
				queries[i].flag = 2;
				threadQuery = i;
				slot = queries[i].slot;
				qFound = 1;

				break;
			}
		}

		// If we found a job
		if (qFound)
		{
			// If user has asked for test solution
			if (queries[threadQuery].test == 1)
			{
				for (i = queries[threadQuery].number; i < queries[threadQuery].number + 10; i++)
				{
					// Wait while the slot has to be read by client
					s_wait(s_slot[slot]); 
					usleep(500000);

					// Put number to client
					s_data->rNumber = 0;
					s_data->slots[slot] = i;
					s_data->qFlag[slot] = 1;

					while (s_data->qFlag[slot] != 0) {}

					s_signal(s_slot[slot]);

					// Update progress of thread
					testProgress += 1;
					int checkProgress = ((testProgress / 30.0) * 100.0);
					printf("Test Progress = %d / 30\n%% Complete = %d\n", testProgress, checkProgress);

					queries[threadQuery].progress = checkProgress;
				}
			}
			// User has entered a number to factor
			else
			{
				if (queries[threadQuery].number <= (long) 0)
				{
					queries[threadQuery].progress = 100;
				}
				else
				{
					// For all the number between 1 and the given number
					for (long z = 1; z <= queries[threadQuery].number; z++)
					{
						// If i is a factor of the given number
						if (queries[threadQuery].number % z == 0)
						{
							// Wait while the slot has to be read by client
							s_wait(s_slot[slot]);

							// Put number to client
							s_data->rNumber = queries[threadQuery].number;
							s_data->slots[slot] = z;
							s_data->qFlag[slot] = 1;

							// Wait for the client to read the number
							while (s_data->qFlag[slot] != 0) {}

							// Signal another thread
							s_signal(s_slot[slot]);
						}

						// Update progress of thread
						if (z == queries[threadQuery].number)
						{
							queries[threadQuery].progress = 100;
							printf("Eyy 100%% for %d yo\n", queries[threadQuery].number);
						}
						else
						{
							queries[threadQuery].progress = ceil((int) (((float) z / queries[threadQuery].number * 100.0)));
						}
					}
				}
			}
		}

		// Thread has finished the job
		queries[threadQuery].flag = 3;
		printf("All Factors for %d found.\n", queries[threadQuery].number);		
	}
}

void runServer(int argc, char * argv[])
{
	printf("\t\tFactorise Server Engaged\n\n");
	printf("\t\tReady to perform some dope factorisations.\n\n");
	printf("\t\tResults will be sent to client\n\n");
	
	// Initialise semaphores for queries/threads
	q_sem = s_get(0);

	for (int i = 0; i < 10; i++)
	{
		s_slot[i] = s_get(1);
	}

	if (argc == 2 && atoi(argv[1]) > 0)
	{
		threadCount = atoi(argv[1]) * 10;
	}

	printf("Thread Pool Size = %d\nThreads Per Query = %d\n", threadCount, threadCount / 10);

	queries = (TQuery *) malloc(sizeof(TQuery) * threadCount);

	// Set shared memory for use with client/server
	if ((id = shmget(KEY, sizeof(Shmem), IPC_CREAT | 0666)) < 0)
	{
		printf("Shared memory could not be created. Exiting...\n");
		exit(1);
	} 

	printf("Shared Memory Allocated!\nSHMEM ID = %d\n", id);

	s_data = (Shmem *)shmat(id, 0, 0);

	if (s_data == (void *)-1)
	{
		printf("Could not set shared memory struct. Exiting...\n");
		exit(1);
	}

	// Initialise threads
	threads = (pthread_t *) malloc(sizeof(pthread_t) * threadCount);

	for (int i = 0; i < threadCount; i++)
		pthread_create(&threads[i], NULL, process_threads, NULL);

	while (1)
	{
		// If conditional has been set to an invalid value
		if (s_data->cHandle != 0 && s_data->cHandle != 1)
		{
			printf("Error receiving client handle information. Exiting...");
			break;
		}

		// If conditional shows there is data to be read
		if (s_data->cHandle == 1)
		{
			// Read data from query slots where a query has been flagged
			for (int i = 0; i < 10; i++)
			{
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

					for (int x = 0; x < qThreads; x++)
					{
						for (int z = 0; z < threadCount; z++)
						{
							if (queries[z].flag == 0)
							{
								queries[z].slot = i;

								// Test query
								if (facNumber == 0)
								{
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
										gettimeofday(&s_data->start[i], NULL);
										queries[z].number = facNumber;
									}
									else 
									{
										queries[z].number = abs((facNumber >> x) | (facNumber << qThreads - x));
									}
								}

								queries[z].flag = 1;
								queries[z].progress = 0;

								// Signal thread to wake up and factorise number
								s_signal(q_sem);

								break;
							}	
						}
					}
					break;
				}
			}
		}

		// Update current progress of each query
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

			// Progress of factorisation query
			if (threads > 0 && qThreads != 3 && s_data->qProcess[i] == 1)
			{
				s_data->qProgress[i] = ceil((int)((float) percent / (float) qThreads));
			}
			// Progress of test query
			if (threads > 0 && qThreads == 3 && s_data->qProcess[i] == 1 && s_data->tProcess == 1)
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