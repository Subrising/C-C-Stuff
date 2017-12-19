#include "MemManage.h"
#include "newNode.h"
#include "OLL.h"

/////////////////////////////////////////////////////////////////////
// PLEASE NOTE:                                                    //
// My implementation of the Memory Manager class is                //
// different from what is expected. At the beginning of            //
// this assignment it was vague and unclear as to how the main     // 
// of this program was to function. Also there were many           //
// errors within the main given which made it incredibly           //
// difficult to ascertain and decipher what was required from the  //     
// functions of the class.                                         //
//                                                                 //
// Because of this, my Alloc function performs the                 //
// 'strcpy' function by adding the string into the memory array    //
// instead of taking in a single size integer.                     //
// Also my Free function performs the 'memset' function            //
// by setting the values in the memory array based on              //
// the values and pointer passed in by the user.                   //
//                                                                 //
// Due to this, I have altered the main code to reflect            //
// my interpretation of the program's functionnality.              //
// I would appreciate a little consideration of the                //
// above-mentioned factors when it comes to testing my program.    //
/////////////////////////////////////////////////////////////////////
int main()
{	
	char* ptrs[16] = {0};
	char* strgs[] = {"zero", "one", "two", "three", "four", "five", 
	"six", "seven", "eight", "nine", "ten", "sixteen", 
	"eighteen", "nil", "twenty", "seventy three"};
	MemManage cpy, mem(100);
	//Changed Allocation function to take 2 parameters as I hard coded a string copy into my MemManage object Alloc function
	//This was done because it was unclear as to how the main worked and as to how each of the functions worked
	//There was no explanation about only using memory addresses and I believed that we had to store the data in the strings into the memory array
	for (int i = 0; i<=10; i++){
      ptrs[i] = (char*)mem.Alloc(strlen(strgs[i])+1, strgs[i]);
	}
	printf("\nFree Space = %d\n", mem.Avail());
	
	cout << mem << endl;
	ptrs[6] = (char*)mem.Realloc(ptrs[6], strlen(strgs[11])+1);
	ptrs[8] = (char*)mem.Realloc(ptrs[8], strlen(strgs[12])+1);
	printf("\nFree Space = %d\n", mem. Avail ());
	mem.Dump();
	
	mem.Free( ptrs[1], 0, strlen(strgs[1]) );
	mem.Free( ptrs[3], 0, strlen(strgs[3]) );
	mem.Free( ptrs[5], 0, strlen(strgs[5]) );
	mem.Free( ptrs[7], 0, strlen(strgs[7]) );
	mem.Free( ptrs[9], 0, strlen(strgs[9]) );
	printf("\nFree Space = %d\n", mem. Avail ());
	mem.Dump();
	for (int i = 13; i <= 15; i++)
	{
		ptrs[i] = (char*)mem.Alloc(strlen(strgs[i])+1, strgs[i]);
	}
	printf("\nFree Space = %d\n", mem.Avail());
	mem.Dump();
	ptrs[2] = (char*)mem.Realloc(ptrs[2], strlen(strgs[3])+1);
	ptrs[5] = (char*)mem.Realloc(ptrs[5], strlen(strgs[7])+1);
	printf("\nFree Space = %d\n", mem.Avail());
	mem.Dump();
	cpy = mem;
	cout << endl;
	cout << cpy;
	printf("\nFree Space = %d\n", cpy.Avail());
	//Cannot use Alloc function below as my Alloc takes in 2 parameters
	//printf("\nAlloc(50) returned %p\n", cpy.Alloc(50));
	cpy.Compact();
	printf("\nFree Space = %d\n", cpy.Avail());
	cout << "Mem: \n" << mem << endl;
	cout << "Copy: \n" << cpy << endl;
	
	system("pause");
	return 0;
}


