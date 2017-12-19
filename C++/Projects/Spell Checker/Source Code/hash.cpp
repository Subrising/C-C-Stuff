#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <fstream>

#include "hash.h"

using namespace std;

// Initializes a hash table with null elements
Hash::Hash()
{
	hashTable = new LinkedHash*[tableSize];
	for (int i = 0; i < tableSize; i++)
	{
		hashTable[i] = NULL;
	}
	hashElements = 1;
}

// Reads in dictionary file and spell checking file
void Hash::readFile(string fileName)
{
	newFile.open(fileName, fstream::in);
	if (!newFile.is_open())
	{
		cout << "File Could Not Open" << endl;
		exit(1);
	}
}

// Creates hash table for dictionary
void Hash::createHash()
{
	char newArr[22];
	while ((newFile.getline(newArr, 22, '\n')))
	{   
		push_back(newArr);
		hashElements++;
	}
}

// Spell checks the words in newEisenhower.txt
void Hash::checkFileWords()
{
	char newArr[1000];
	while ((newFile.getline(newArr, 22, '\n')))
	{   
		if (!containsWord(newArr))
		{
			PrintSimilarWords(newArr, 1);
			system("pause");
			cout << endl;
		}
	}
}

// Adds an element from the dictionary to the dictionary hash table
// This method checks to see if the hash table already contains the word
// to be added. If it does, the word is not added. If it does not, the 
// word is added.
void Hash::push_back(char * word)
{
	int hashIndex = hashFunction(word);
	if (hashTable[hashIndex] == NULL)
	{   
		char * cp = new char[strlen(word)+1];
		strcpy(cp, word);
		hashTable[hashIndex] = new LinkedHash(hashIndex, cp);
	}
	else
	{
		if (!containsWord(word))
		{
			LinkedHash * entry = hashTable[hashIndex];
			while (entry->next != NULL)
				entry = entry->next;
			char * cp = new char[strlen(word)+1];
			strcpy(cp, word);
			entry->next = new LinkedHash(hashIndex, cp);
		}
	}
}

// This method searches through the whole hash table to see if it contains
// a specific word. If it does, it returns true. If it does not, it 
// returns false.
bool Hash::containsWord(char * word)
{
	int hashIndex = hashFunction(word);
	if (hashTable[hashIndex] == NULL)
	{
		return false;
	}
	else
	{
		LinkedHash * entry = hashTable[hashIndex];
		while (entry != NULL && strcmp(entry->value, word) != 0)
			entry = entry->next;
		if (entry == NULL)
			return false;
		else
			return true;
	}
	return false;
}

// This method uses the djb2 algorithm to create hashing values
// to be used to store elements in the hash table.
int Hash::hashFunction(char * key)
{
	unsigned long hash = 5381;
	int c;

	while (c = *key++)
		hash = hash * 33 + c;

	return hash % tableSize;
}

// This method goes through every word in the hash table and compares it 
// with a specific word that needs spell checking. It gets the distance
// between the currently checked word in the hash table and the word to
// spell check and if the returned distance has a value of less than the
// maximum distance passed to the method, it is added to similarWords
// vector. 

// After every word in the hash table has been checked, the elements
// in the similarWords vector are printed out.
void Hash::PrintSimilarWords(char * word, int maxDist)
{
	//My modified Levenshtein Distance on words
	//Loop through whole hashmap and add to a vector words that have a small distance to the word being compared to.
	//Prints those words out.

	vector<char *> similarWords;
	int levDist = 0;
	for (int i = 0; i < tableSize; i++)
	{
		LinkedHash * entry = hashTable[i];
		while (entry != NULL)
		{
			levDist = distance(entry->value, word);
			if (levDist <= maxDist)
				similarWords.push_back(entry->value);
			entry = entry->next;
		}
	}
	if (similarWords.size() == 0 && maxDist < 4)
		PrintSimilarWords(word, maxDist + 1);
	else
	{
		cout << word << " is not a valid word." << endl;
		if ((int)similarWords.size() == 0)
			cout << "Failed to find any similar words." << endl << endl;
		else
		{
			cout << "Words you may have meant: " << endl;
			for (int i = 0; i < (int)similarWords.size(); i++)
				if (i != 5)
				{
					cout << similarWords[i] << endl;
				}
				else
					break;
			cout << endl;
		}
	}
	return;
}

// This method calculates the distance between two words.
// It does this by comparing the amounts of each letter (a-z)
// both words have along with the word sizes.
// The differences are totalled and returned.
int Hash::distance(const char * word1,
	const char * word2)
{
	int word1Arr[26];
	int word2Arr[26];
	for (int i = 0; i < 26; i++)
	{
		word1Arr[i] = 0;
		word2Arr[i] = 0;
	}

	for (int i = 0; i < (int)strlen(word1); i++)
		if (word1[i] % 97 < 26)
			word1Arr[word1[i] % 97]++;

	for (int i = 0; i < (int)strlen(word2); i++)
		if (word2[i] % 97 < 26)
			word2Arr[word2[i] % 97]++;

	int dist = 0;

	for (int i = 0; i < 26; i++)
		dist = dist + abs(word1Arr[i] - word2Arr[i]);

	int length = strlen(word1) - strlen(word2);
	dist += abs(length);

	return dist;
}
