/*#ifndef CLIENT_H
#define CLIENT_H  

// Headers
#include <stdio.h>
#include <stdlib.h>

//#include <termios.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

//#include <sys/time.h>
#include <sys/types.h>
//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <sys/errno.h>

#include <pthread.h>
#include <math.h>
#include <WinSock2.h>

#include <signal.h>

// Constant Values
#define STDIN_FILENO 0

#define NB_ENABLE 1
#define NB_DISABLE 0

#define KEY 8080

// Shared Memory Struct
typedef struct 
{
	// Values
	int number;
	int rNumber;            
	int rNumbers[32];
	int slots[10];   
	int sNumber;
	struct timeval start[10];
	struct timeval end[10];

	// Checks
	int cHandle;        
	char qFlag[10];   
	int qProgress[10];   
	int processing;   
	int qProcess[10];

} Shmem;

// Query Struct
typedef struct
{
	// Number and rotated numbers to factorise
	int number;  
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

// Functions
void runClient(int argc, char * argv[]);
int kbhit();
void clear_all();


#endif
*/

#include "stdafx.h"