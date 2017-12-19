#include <iostream>
#include <vector>
#include <string>
#include <random>
#ifndef NEWNODE_HEADER
#define NEWNODE_HEADER
using namespace std;


class newNode
{
 public:
	int length;
	newNode * next;
	char * memPt;
	newNode(int l, newNode * p, char * data);
	newNode(const newNode & n);
	newNode & operator = (const newNode & n);
};

#endif
