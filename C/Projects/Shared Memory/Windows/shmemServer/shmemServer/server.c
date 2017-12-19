#include "stdafx.h"
/*#include "server.h"

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
			if (queries[threadJob].test == 1)
			{
				for (i = queries[threadJob].number; i < queries[threadJob].number + 10; i++)
				{
					// Wait while the slot has to be read by client
					s_wait(s_slot[slot]); 
					//usleep(500000);

					// Put number to client
					s_data->slots[slot] = i;
					s_data->qFlag[slot] = 1;

					while (s_data->qFlag[slot] != 0) {}

					s_signal(s_slot[slot]);

					testProgress += 1;
					int checkProgress = ((testProgress / 30.0) * 100.0);
					printf("Test Progress = %d / 30\n%% Complete = %d\n", testProgress, checkProgress);

					queries[threadJob].progress = checkProgress;
					
				    //}
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
					for (i = 1; i <= queries[threadJob].number; i++)
					{
						// If i is a factor of the given number
						if (queries[threadJob].number % i == 0)
						{
							// Wait while the slot has to be read by client
							s_wait(s_slot[slot]);

							// Put number to client
							s_data->rNumber = queries[threadJob].number;
							s_data->slots[slot] = i;
							s_data->qFlag[slot] = 1;

							// Wait for the client to read the number
							while (s_data->qFlag[slot] != 0) {}

							// Signal another thread
							s_signal(s_slot[slot]);
						}

						// Update progress of thread
						if (i == queries[threadJob].number)
						{
							queries[threadJob].progress = 100;
							printf("Eyy 100%% for %d yo\n", queries[threadJob].number);
						}
						else
						{
							queries[threadJob].progress = ceil((int) (((float) i / (float) queries[threadJob].number * 100.0)));
						}
					}
				}
			}
		}

		// Thread has finished the job
		queries[threadJob].flag = 3;
		printf("All Factors for %d found.\n", queries[threadJob].number);
		//s_data->qProgress[slot] += (int)(1.0 / 32.0 * 100.0);
		//if (threadJob % (threadCount / 10) - 1 == 0)
		//	queries[threadJob].progress = 100;
		//printf("Query Progress for slot %d = %d\n", slot, s_data->qProgress[slot]);
		//s_data->qFlag[slot] = 0;
		
	}
}

void runServer(int argc, char * argv[])
{
	printf("\t\tFactorise Server Engaged\n\n");
	printf("\t\tReady to perform some dope factorisations.\n\n");
	printf("\t\tResults will be sent to client\n\n");
	
	q_sem = s_get(0);

	for (int i = 0; i < 10; i++)
	{
		s_slot[i] = s_get(1);
	}

	if (argc == 2 && atoi(argv[1] > 0))
	{
		threadCount = atoi(argv[1]) * 10;
	}

	printf("Thread Pool Size = %d\nThreads Per Query = %d\n", threadCount, threadCount / 10);

	queries = (TQuery *) malloc(sizeof(TQuery) * threadCount);

	TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       
		sizeof(s_data),           
		szName);                 // name of mapping object


	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
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
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}

	threads = (pthread_t *) malloc(sizeof(pthread_t) * threadCount);

	for (int i = 0; i < threadCount; i++)
		pthread_create(&threads[i], NULL, process_threads, NULL);

	while (1)
	{
		if (s_data->cHandle != 0 && s_data->cHandle != 1)
		{
			printf("Error receiving client handle information. Exiting...");
			break;
		}

		if (s_data->cHandle == 1)
		{
			//printf("HELLO! ");
			for (int i = 0; i < 10; i++)
			{
				if (s_data->slots[i] == 0 && s_data->qFlag[i] != 1)
				{
					int facNumber = s_data->number;

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

								if (facNumber == 0)
								{
									GetLocalTime(&s_data->start[i], NULL);
									queries[z].test = 1;
									queries[z].number = x * 10;
								}
								else
								{
									if (x == 0)
									{
										GetLocalTime(&s_data->start[i], NULL);
										queries[z].number = facNumber;
									}
									else 
									{
										queries[z].number = (facNumber >> x) | (facNumber << qThreads - x);
									}
								}

								queries[z].flag = 1;
								queries[z].progress = 0;

								s_signal(q_sem);

								break;
							}	
						}
					}
					break;
				}
			}
		}

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

			if (threads > 0 && qThreads != 3 && s_data->qProcess[i] == 1)
			{
				s_data->qProgress[i] = ceil((int)((float) percent / (float) qThreads));
				//printf("qProgress[%d] set to %d\n", i, s_data->qProgress[i]);
			}
			else if (threads > 0 && qThreads == 3 && s_data->qProcess[i] == 1)
			{
				s_data->qProgress[i] = ((testProgress / 30.0) * 100.0);
				//printf("qProgress[%d] set to %d\n", i, s_data->qProgress[i]);
			}
				
				//s_data->qProgress[i] = ceil((percent) / (float) (qThreads * 10));
		}	
	}
}
*/