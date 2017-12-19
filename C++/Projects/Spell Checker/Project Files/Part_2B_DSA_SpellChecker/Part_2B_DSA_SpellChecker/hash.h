#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>

using namespace std;

#ifndef HASH_H
#define HASH_H

// Linked list class for chaining in hash table
class LinkedHash
{
public:
	int key;
	char * value;
	LinkedHash * next;
	LinkedHash(int key, char * value)
	{
		this->key = key;
		this->value = value;
		this->next = NULL;
	}
};

// Main hash table class
class Hash
{
public:
	// Attributes
	static const int tableSize = 997;
	LinkedHash ** hashTable;
	int hashElements;
	fstream newFile;

	// Constructor
	Hash();

	// File reading method
	void readFile(string fileName);

	// Hash table creation from file method
	void createHash();

	// Method that creates hash values for words
	int hashFunction(char * key);

	// Hash table insertion method
	void push_back(char * word);

	// File spell checking method
	void checkFileWords();

	// Hash table search to see if it contains
	// a specific word method
	bool containsWord(char * word);

	// Spell checking and word suggestion methods
	void PrintSimilarWords(char * word, int maxDist);
	int distance(const char * word1, const char * word2);
};

#endif