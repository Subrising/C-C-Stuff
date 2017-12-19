#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "BSTree.h"

using namespace std;

// BSTree Constructors
BSTree::BSTree()
{
	elements = 0;
	root = NULL;
}

BSTree::BSTree(string data)
{
	elements = 0;
	root = GetNewNode(data);
}

// Opens file to read 
void BSTree::readFile(string newFile)
{
	myFile = fopen(newFile.c_str(), "r");
	if (myFile == NULL) 
	{ 
		printf("Could not open file!\n"); 
		exit(1);
	}
}

// Opens file to write to
void BSTree::writeFile(string newFile)
{
	outFile = fopen(newFile.c_str(), "w");
	if (outFile == NULL) 
	{ 
		printf("Could not open file!\n"); 
		exit(1);
	}
}

// Reads through open file and adds each word to a string.
// This method removes words with errant punctuation or
// words consisting of numbers. 

// This method is used on Ass2 Dictionary.txt
void BSTree::populateString()
{
	char c;
	while ((c = fgetc(myFile)) != EOF)
	{   
		// Gets the character after the character stored in c
		// This is needed because the current character and the character
		// after it needs to be checked to remove unwanted punctuation.
		char nextChar = fgetc(myFile);

		if (isdigit(c))
			c = ' ';

		if (c == '-' &&  nextChar == '\n')
			fgetc(myFile);

		if (c == '-' &&  nextChar == '-')
		{
			c = ' ';
			nextChar = ' ';
		}

		if (c == ' ' &&  nextChar == '-')
		{
			fgetc(myFile);
			nextChar = ' ';
		}

		if (ispunct(c) && c != '-' && c != '\'')
			c = '\0';

		if (c == ' ' && nextChar == '\'')
			nextChar = '\0';

		if (c == '\'' && nextChar == ' ')
			c = '\0';

		ungetc(nextChar, myFile);

		if (c != '\0')
			fileElements += tolower(c);
	}
}

// This method goes through Eisenhower Spell.txt and adds each word
// to a string. This method does not remove errant punctuation.
void BSTree::populateString2()
{
	char c;
	while ((c = fgetc(myFile)) != EOF)
	{   
		char nextChar = fgetc(myFile);
		if(c == '-' &&  nextChar == '\n')
		{
			fgetc(myFile);
		}
		ungetc(nextChar, myFile);
		fileElements += tolower(c);
	}
}

void BSTree::deleteString()
{
	fileElements = "";
}

// This method is used to write the words stored into the string
// from the populateString2() method (Eisenhower Spell.txt) into
// a new file used in the spell checking program (newEisenhowerSpell.txt)
void BSTree::writeToFile()
{
	string temp;
	for(int i = 0; i < (int)fileElements.length(); i++)
	{
		if(fileElements[i] != ' ')
		{
			temp += fileElements[i];
		}
		else 
		{  
			fprintf(outFile, "%s\n", temp.c_str());
			temp.clear();
		}
	}
}

// This method populates the binary search tree with the words from
// the words stored in the string from the populateString() method.
// This methods checks to see if a word in the string contains unwanted 
// characters and if it does, that word is not added to the tree.
void BSTree::populateTree()
{
	string temp;
	for(int i = 0; i < (int)fileElements.length(); i++)
	{
		if(fileElements[i] != ' ')
		{
			temp += fileElements[i];
		}
		else 
		{  
			// Checks to see if an unwanted character is in the word
			// currently being checked
			size_t nfound = temp.find('\n');
			size_t tfound = temp.find('\t');
			size_t vfound = temp.find('\v');
			size_t bfound = temp.find('\b');
			size_t rfound = temp.find('\r');
			size_t ffound = temp.find('\f');
			size_t afound = temp.find('\a');

			// If there is no unwanted character found in the word
			// currently being checked, it is output to the dict.txt
			// text file
			if (    nfound == string::npos 
				&& tfound == string::npos 
				&& vfound == string::npos 
				&& bfound == string::npos 
				&& rfound == string::npos 
				&& ffound == string::npos 
				&& afound == string::npos )
			{
				if (!temp.empty() && temp.find_first_of('\0') != 0)
					Insert(GetRoot(), temp);   
			} 
			temp.clear();
		}
	}
}

void BSTree::closeInput()
{
	fclose(myFile);
}

void BSTree::closeOutput()
{
	fclose(outFile);
}

// This method uses recursion to search through the binary search
// tree to see if the word passed to the method is already in the
// tree.
bool BSTree::Search(BSTree * rootPtr, string data)
{
	if (rootPtr == NULL)
		return false;
	else if (rootPtr->word == data)
		return true;
	else if (data <= rootPtr->word)
		return Search(rootPtr->left, data);
	else
		return Search(rootPtr->right, data);
}

// This method traverses the binary search tree to find
// the correct insertion point for the word to be added.
BSTree * BSTree::Insert(BSTree * rootPtr, string data)
{
	bool contains = Search(rootPtr, data);
	if (contains)
		return NULL;
	if (rootPtr == NULL)
		rootPtr = GetNewNode(data);
	else if (data <= rootPtr->word)
		rootPtr->left = Insert(rootPtr->left, data);
	else
		rootPtr->right = Insert(rootPtr->right, data);
	return rootPtr;
}

// This method creates the node of the next element
// that is being added to the binary search tree.
BSTree * BSTree::GetNewNode(string data)
{
	BSTree * newNode = new BSTree();
	newNode->word = data;
	newNode->left = NULL;
	newNode->right = NULL;
	elements++;
	return newNode;
}

// This method traverses the binary search tree to find the
// maximum height.
int BSTree::FindHeight(BSTree * rootPtr)
{
	if (rootPtr == NULL)
		return -1;
	return max(FindHeight(rootPtr->left), FindHeight(rootPtr->right)) + 1;
}

BSTree * BSTree::GetRoot()
{
	return root;
}

// This method traverses the binary search tree in order, and
// prints out each element to the dict.txt file. 
// The in order traversal writes out the elements in the tree
// in alphabetical order.
void BSTree::InOrder(BSTree * rootPtr)
{
	if (rootPtr == NULL)
		return;
	InOrder(rootPtr->left);
	if (rootPtr->word != "m" && !rootPtr->word.empty())
		fprintf(outFile, "%s\n", rootPtr->word.c_str());
	InOrder(rootPtr->right);
}