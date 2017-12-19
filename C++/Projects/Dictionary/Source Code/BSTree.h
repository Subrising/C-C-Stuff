#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#ifndef BST_H
#define BST_H

class BSTree
{
public:
	// Attributes
	FILE * myFile;
	FILE * outFile;
	string word;
	string fileElements;
	int elements;
	BSTree * root;
	BSTree * left;
	BSTree * right;

	// Constructors
	BSTree();
	BSTree(string data);

	// File methods
	void readFile(string fileName);
	void writeFile(string fileName);

	void closeInput();
	void closeOutput();

	void InOrder(BSTree * rootPtr);
	void writeToFile();

	// Populating strings & binary search tree methods
	void populateString();
	void populateString2();

	void populateTree();

	// String deletion
	void deleteString();

	// Binary search tree node creation and insertion
	BSTree * GetNewNode(string data);
	BSTree * Insert(BSTree * rootPtr, string data);

	// Binary search tree node search method
	bool Search(BSTree * rootPtr, string data);

	BSTree * GetRoot();

	// Binary search tree maximum height method
	int FindHeight(BSTree * rootPtr);

};

#endif