#include "MemManage.h"
#include "OLL.h"
#include <iomanip>
using namespace std;


MemManage::MemManage()	
{   
	arrSize = 0;
	freeMem = 0;
}	

//Initializes MemManage object, sets array size and initializes all elements in array to 0.
MemManage::MemManage(int maxSize=0)	
{   
	arrSize = maxSize;
	freeMem = maxSize;
	freeList = OLL();
	takenList = OLL();
	memArr = new char[arrSize];
	for (int i = 0; i<arrSize; i++)
		memArr[i] = 0;
}		

MemManage::~MemManage()			
{
	freeList.clear();
	takenList.clear();
	delete [] memArr;
}					

//Takes in a length and a memory location and stores the string into the memory array.
//First checks through the free list to see if there is an element that the string can fit into.
//If there isn't the string is added after the last taken element.
void* MemManage::Alloc(int size, char * string)		
{   
	char * address = NULL;
	if (takenList.head == NULL)
	{ 
		address = &memArr[0];
		takenList.taken_push_back(size, &memArr[0]);
		for(int i = 0; i<size; i++)
		{
			memArr[i] = *string;
			string += 1;
		}
		freeMem -= size;
		return address;
	}
	else
	{
		if(freeList.head == NULL)
		{ 
			if(size>freeMem)
			{
				cout << "Insufficient Memory" << endl;
				return 0;
			}
			else 
			{
				for (int i=(arrSize-freeMem); i<arrSize-1; i++)
				{     
					address = &memArr[i];
					takenList.taken_push_back(size, &memArr[i]);
					for(int j = 0; j<size; j++, i++)
					{
						memArr[i] = *string;
						string += 1;
					}
					freeMem -= size;

					return address;
				}
			}
		}
		else 
		{ 
			newNode * memNode = freeList.searchFree(size);
			if (memNode == NULL)
			{
				for (int i=0; i<arrSize-1; i++)
				{
					if (memArr[i] == NULL && memArr[i+1] == NULL)
					{ 
						if(i+size >= arrSize-1)
						{
							cout << "Insufficient Memory" << endl;
							return 0;
						}
						else
						{
							i++;
							address = &memArr[i];
							takenList.taken_push_back(size, &memArr[i]);
							for(int j = 0; j<size; j++, i++)
							{
								memArr[i] = *string;
								string += 1;
							}
							freeMem -= size;
							return address;
						}
					}
				}
			}
			else
			{ 
				char * strAddress = memNode->memPt;
				address = memNode->memPt;
				takenList.taken_push_back(size, address);
				for(int i = 0; i<size; i++)
				{
					*(strAddress+i) = *(string+i);
				}
				freeMem -= size;
				freeList.remove(address);
				return address;
			}
		}
	}
}	

///////////////////////////////////////////////////////////////
//  This function finds the piece of memory within            // 
//  the memory array to where the pointer points             // 
//  to and then loops through the length size and            // 
//  replaces the data stored in those memory                 //
//  locations to the integer value that was passed through.  //
///////////////////////////////////////////////////////////////
void MemManage::Free(char * freeLocation, int value, int length)
{
	freeList.push_back(length+1, freeLocation);
	takenList.remove(freeLocation);

	for (int i = 0; i < length; i++)
	{
		*(freeLocation + i) = value;
	}
	freeMem += length;
	freeList.appendFree();
}

////////////////////////////////////////////////////////////////////////////
//  The function calls the linked list reallocation address function      //
//  within the MemManage class. The reallocation address function first   //
//  finds the node within the taken list that matches the memory pointer  //
//  that is passed through. From that it checks to see if there is        //
//  enough space after the memory location to increase its current        //
//  length to the new integer length that is passed through.              //
//  If there is, the free memory list and taken memory list are updated.  //
//  If there is not enough space to increase its size at the current      //
//  location, the memory is freed and then a new place within the memory  //
//  array is found to store the data after calling the allocation         //
//  function on the data.                                                 //
////////////////////////////////////////////////////////////////////////////
void* MemManage::Realloc(void* reAllocPt, int size)	
{   
	char * checkSame = NULL;
	checkSame = ReAllocAddress((char*)reAllocPt, size);

	if(reAllocPt == checkSame)
	{ 
		return reAllocPt;		
	}
	else
	{  
		newNode * free = takenList.searchFree((char*)reAllocPt);
		int newLength = free->length;
		char * str = free->memPt;
		char * temp = new char[size];
		for (int i = 0; i < size; i++)
			temp[i] = 0;
		str = free->memPt;
		for (int i = 0; i < newLength; i++, str++)
			temp[i] = *str;

		Free(free->memPt, 0, free->length-1);	
		checkSame = (char*)Alloc(size, temp);
	}
	return checkSame;
}

////////////////////////////////////////////////////////////////////
//  Defragments the memory array within the MemManage class by    //
//  creating a second array and looping through the taken memory  //
//  list and taking the contents of each node in the list and     //
//  stores that within the new array, removing all spaces and     //
//  packing the data. The old array is then deleted and the new   //
//  array created becomes the memory array.                       //
////////////////////////////////////////////////////////////////////
void MemManage::Compact()			
{
	char * compactArray = takenList.storeNewArray(arrSize);
	freeList.clear();
	delete [] memArr;
	memArr = compactArray;
	freeMem = Avail();
}				

int MemManage::Avail() const					
{
	return takenList.returnFree(arrSize);
}	

void MemManage::Dump() const				
{
	for (int i = 0, lineCheck = 1; i < arrSize; i++, lineCheck++)
	{
		cout << setfill('0') << setw(2) << hex << (int) memArr[i] << " ";
		if (lineCheck == 16)
		{
			lineCheck = 0;
			cout << endl;
		}
	}
	cout << endl;
}

ostream& operator << (ostream& out, const MemManage & printArr)
{
	printArr.Dump();
	return out;
}

char * MemManage::getArr() const
{
	return memArr;
}

///////////////////////////////////////////////////////////////////////
//  Takes the MemManage object that is passed through and copies     //
//  its data into the MemManage object that wants to equal it.       //
//  To make a deep copy of the memory arrays from the new object     //
//  and the passed through object, a second memory array is created  //
//  to copy the data values and pointers stored in the passed        //
//  through objects array so that the new object that is going to    //
//  store the passed through objects details does not point to the   //
//  same memory locations for their memory arrays. Once this is      //
//  complete the new objects original memory array is deleted and    //
//  replaced byt the second memory array that was just created and   //
//  the free and taken lists are updated accordingly.                //
///////////////////////////////////////////////////////////////////////
MemManage & MemManage::operator=(const MemManage & oldArr) 
{  
	int tempLength = 0;
	freeList.clear();
	takenList.clear();
	arrSize = oldArr.arrSize;
	freeMem = oldArr.freeMem;
	memArr = new char[arrSize];
	for (int i = 0; i < arrSize; i++)
		memArr[i] = 0;
	char * oldArrData = oldArr.getArr();
	newNode * current = oldArr.takenList.head;
	int i = 0;
	for(; i < arrSize - 1;)
	{ 
		tempLength = 0;
		while(!(*(oldArrData + i) == NULL && *(oldArrData + i + 1) == NULL))
		{ 
			*(memArr+i) = *(oldArrData+i);
			tempLength++;
			i++;
		}
		takenList.taken_push_back(tempLength+1, &memArr[i-tempLength]);

		if (i+1 < arrSize && *(oldArrData+i) == NULL && *(oldArrData+i+1) == NULL)
		{
			while(*(oldArrData+i+1) == NULL)
			{
				i++;
				tempLength++;
				i++;
			}
			if (!(tempLength == oldArr.Avail()))
			{ 
				freeList.push_back(tempLength, &memArr[i-tempLength+1]);
			}
		}
	}
	return *this;
}

void MemManage::toString()
{
	for (int i = 0; i<arrSize; i++)
	{
		if(memArr[i] != NULL)
			cout << memArr[i];
		else 
			cout << " ";
	}
	cout << endl;
}

//Reallocates the memory from the current location.
char * MemManage::ReAllocAddress(char * reAllocPt, int size)
{   
	bool found = false;
	bool spaceAvail = true;
	newNode * current = takenList.head;
	char * keepLoc = NULL;
	char * newLoc = NULL;
	char * freeLoc = NULL;
	newNode * freeNode = NULL;
	int reduceFreeSize = 0;
	while (current != NULL && !found)
	{
		if (current->memPt == reAllocPt)
			found = true;
		else 
			current = current->next;
	}
	reduceFreeSize = size - current->length;
	keepLoc = current->memPt;
	newLoc = keepLoc;
	keepLoc += current->length;
	for (int i = 0; i < (size - current->length); i++, keepLoc++)
	{
		if (*keepLoc != NULL)
		{
			spaceAvail = false;
			break;
		}
	}
	if (spaceAvail)
	{ 
	  keepLoc = newLoc;
	  for (int i = 0; i < size-1; i++)
        {
		  if (keepLoc == NULL && keepLoc+1 == NULL)
		  {
			keepLoc++;
			break;
		  }
		  keepLoc++;
	    }
	  freeLoc = keepLoc;
	  freeNode = freeList.searchFree(freeLoc);
	  if (freeNode != NULL)
	  {
		  freeNode->length -= reduceFreeSize;
		  if(freeNode->length == 0)
		  {
			freeList.remove(freeLoc);
			freeMem -= reduceFreeSize;
		  }
		  else
		  { 
			int newNodeLength = freeNode->length;
		    freeList.remove(freeLoc);
			freeList.push_back(newNodeLength, freeLoc);
		  }
	  }
	  takenList.remove(newLoc);
	  takenList.taken_push_back(size, newLoc);
	  return newLoc;
	}
	else
	  return newLoc + 1;
}

void MemManage::deleteObject()
{
	freeList.clear();
	takenList.clear();
	delete [] memArr;
}