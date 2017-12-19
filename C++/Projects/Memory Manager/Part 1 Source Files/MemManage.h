#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "OLL.h"

class MemManage 
{ 
  public:
	  int arrSize;
	  int freeMem;
	  OLL freeList;
	  OLL takenList;
	  MemManage();
	  MemManage(int maxsize);		
	  ~MemManage();					
	  void* Alloc(int size, char *);	
	  void Free(char *, int, int);			
	  void* Realloc(void*, int);	
	  void Compact();				
	  int Avail() const;					
	  void Dump() const;					
      char * getArr() const;
	  MemManage & operator = ( const MemManage &);
	  friend ostream& operator << (ostream&, const MemManage &);
	  void toString();
	  char * ReAllocAddress(char * reAllocPt, int size);
	  void deleteObject();
  
  private:
	  char * memArr;
};