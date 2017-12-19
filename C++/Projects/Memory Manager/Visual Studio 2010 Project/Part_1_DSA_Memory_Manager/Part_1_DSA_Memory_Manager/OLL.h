#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "newNode.h"
#ifndef OLL_HEADER
#define OLL_HEADER
using namespace std;


class OLL
{
public:
	OLL();
	~OLL();
	void push_back(int, char *);
	void clear();
	void remove(char *);
	newNode * searchFree(int checkLength); 
	newNode * searchFree(char * freeLoc); 
	char * storeNewArray(int arrSize);
	int returnFree(int) const; 
	void printList();
	void appendFree();
	void taken_push_back(int, char * );
	newNode * head;
	int len;
};

#endif
