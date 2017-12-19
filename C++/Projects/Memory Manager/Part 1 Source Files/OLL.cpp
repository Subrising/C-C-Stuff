#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "OLL.h"
using namespace std;

OLL::OLL()
{
	head = NULL;
	len = 0;
}

OLL::~OLL()
{
	clear(); 
}

//Push back function for free list. Sorts free list by size of memory available.
void OLL::push_back(int length, char * data)
{
	if(len == 0) 
	{
		head = new newNode(length, 0, data); 
	}
	else if(length <= head->length) 
	{
		newNode * temp = head;
		head = new newNode(length, temp, data); 
	}
	else 
	{
		newNode * p = head;
		newNode * pLink = p->next;
		while(pLink && length > pLink->length) 
		{
			p = pLink;
			pLink = p->next;
		}
		if(!pLink) 
		{
			p->next = new newNode(length, NULL, data); 
		}
		else 
		{
			p->next = new newNode(length, pLink, data);
		}
	}
	len++;
}

//Push back function for taken list. Sorts taken list by memory address.
void OLL::taken_push_back(int length, char * data)
{
	if(len == 0) 
	{
		head = new newNode(length, 0, data); 
	}
	else if(data <= head->memPt) 
	{
		newNode * temp = head;
		head = new newNode(length, temp, data); 
	}
	else 
	{
		newNode * p = head;
		newNode * pLink = p->next;
		while(pLink && data > pLink->memPt) 
		{
			p = pLink;
			pLink = p->next;
		}
		if(!pLink) 
		{
			p->next = new newNode(length, NULL, data); 
		}
		else 
		{
			p->next = new newNode(length, pLink, data);
		}
	}
	len++;
}

//Removes all elements from list
void OLL::clear()
{
	while(head != NULL)
	{
		newNode * tmp = head->next;
		delete head;
		head = tmp;
	}
	len = 0;
	head = NULL;
}

//Removes single element from list
void OLL::remove(char * removeNode)
{   
	newNode * current = head;

	if(len == 1)
	{
		delete current;
		len = 0;
	}
	else 
	{
		bool found = false;
		newNode * trailCurrent = head;
		current = head->next;
		while (current != NULL && !found) 
		{
			if (current->memPt != removeNode)
			{
				trailCurrent = current; 
				current = current->next;
			}
			else
				found = true;
		}
		if (found)
		{
			trailCurrent->next = current->next;
			len--;
			delete current;
		}
		else
			cout << "No item found to remove from memory." << endl;
	}
}

//Searches through list by memory size.
newNode * OLL::searchFree(int checkLength) 
{ 
	bool found = false;
	newNode * current = head; 
	while (current != NULL && !found)
	{ 
		if (current->length >= checkLength)
			found = true; 
		else 
			current = current->next; 
	} 
	if (found) 
	{ 
		return current;
	}
	else
		return NULL;

}

//Searches through list by memory location.
newNode * OLL::searchFree(char * freeLoc) 
{ 
	bool found = false; 
	newNode * current = head; 
	while (current != NULL && !found)
	{ 
		if (current->memPt == freeLoc) 
			found = true; 
		else 
			current = current->next; 
	} 
	if (found) 
	{
		return current;
	}
	else
		return NULL;

}

//Copies data from current memory array into a new memory array.
char * OLL::storeNewArray(int arrSize)
{
	char * keepLoc = NULL;
	int keepCurrentLoc = 0;
	char * newArray = new char[arrSize];
	for (int i = 0; i<arrSize; i++)
	  newArray[i] = 0;
	newNode * current = head; 
	while (current != NULL)
	{ 
		keepLoc = current->memPt;
		for (int i = 0; i < current->length; i++)
		{
			if (i == 0)
				current->memPt = &newArray[keepCurrentLoc];
			newArray[keepCurrentLoc] = *keepLoc;
			keepLoc++;
			keepCurrentLoc++;
		}
		current = current->next;
	}
	return newArray;
}

//Returns the total amount of free memory.
int OLL::returnFree(int totalSize) const
{ 
  int returnTotal = totalSize; 
  newNode * current = head; 
  while (current != NULL) 
  { 
	returnTotal -= current->length; 
	current = current->next; 
  } 
  return returnTotal; 
}

//Prints out the given list.
void OLL::printList()
{   
	newNode * current = head;
	while (current != NULL)
	{
		cout << current->length << endl;
		current = current->next;
	}
}

void OLL::appendFree()
{
	newNode * trailer = head;
	newNode * current = head->next;
	
	while (current != NULL)
	{
		if ((trailer->memPt + trailer->length + 1) == current->memPt)
		{
			trailer->length += current->length;
			remove(current->memPt);
		}
		trailer = current;
		current = current->next;
	}
}