#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>
#include <ostream>
#include <locale>
#include <vector>
#include <algorithm>
#include <istream>

#include "hash.h"

using namespace std;

int main()
{
	int count = 0;
	Hash dictionaryHash = Hash();

	string fileName;

	// Reads in the dictionary file (dict.txt)
	cout << "Please enter file name for dictionary (remember to add .txt)" << endl;
	getline(cin, fileName);

	dictionaryHash.readFile(fileName);

	dictionaryHash.createHash();

	dictionaryHash.newFile.close();

	cout << "Hash Table Created" << endl;
	cout << "Hash Table Elements = " << dictionaryHash.hashElements << endl << endl;

	// Reads in the file to spell check (newEisenhowerSpell.txt)
	cout << "Please enter name of file to spell check (remember to add .txt)" << endl;
	getline(cin, fileName);

	dictionaryHash.readFile(fileName);

	dictionaryHash.checkFileWords();

	dictionaryHash.newFile.close();

	system("pause");
	return 0;
}