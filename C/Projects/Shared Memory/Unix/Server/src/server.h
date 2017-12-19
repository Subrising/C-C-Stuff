#ifndef SERVER_H
#define SERVER_H

// Headers
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <math.h>

// Constant Values
#define KEY 8080

// Semaphore Stuff
typedef struct
{
	pthread_mutex_t * mutex;
	pthread_cond_t * cd;
	int value;
} Semaphore;

Semaphore * s_get(int value)
{
	Semaphore * s = (Semaphore *) malloc(sizeof(Semaphore));

	s->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t)); 
	pthread_mutex_init(s->mutex, NULL);

	s->cd = (pthread_cond_t *) malloc(sizeof(pthread_cond_t)); 
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

int threadCount = 320;		// Maximum thread count of 320. 32 Threads for each potential query, with maximum of 10 queries
pthread_t * threads = NULL; // Threads pointer

TQuery * queries = NULL;    // Queries Struct
Semaphore * q_sem = NULL;   // Semaphore for list of queries
Semaphore * s_slot[10];  	// Semaphore check for maximum number of queries

// Function headers
void runServer(int argc, char * argv[]);
void * process_threads(void * pholder);
void clear_all();

#endif