#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "BSTree.h"


int main()
{
	// Creates a binary search tree with a root node of m
	BSTree dictionary = BSTree("m");
	string fileName; 

	//Reads in Ass2 Dictionary.txt to create dictionary for the spell checking program
	cout << "Enter in file name to read (remember to include .txt)" << endl;
	getline(cin, fileName);
	cout << endl;
	dictionary.readFile(fileName);

	//Outputs the contents from Ass2 Dictionary to dict.txt for the dictionary used in the spell checking program
	cout << "Enter in file name to write to (remember to include .txt)" << endl;
	getline(cin, fileName);
	cout << endl;
	dictionary.writeFile(fileName);

	dictionary.populateString();
	dictionary.populateTree();

	dictionary.InOrder(dictionary.GetRoot());

	dictionary.closeInput();
	dictionary.closeOutput();

	cout << "Total Nodes in Tree = " << dictionary.elements << endl; 
	cout << "Maximum Height of Tree = " << dictionary.FindHeight(dictionary.GetRoot()) << endl;

	dictionary.deleteString();

	//Reads in Eisenhower Spell.txt to create dictionary for the spell checking program
	cout << "Enter in file name to read (remember to include .txt)" << endl;
	getline(cin, fileName);
	cout << endl;
	dictionary.readFile(fileName);

	//Outputs the contents from Eisenhower Spell to newEisenhowerSpell.txt for the Eisenhower used in the spell checking program
	cout << "Enter in file name to write to (remember to include .txt)" << endl;
	getline(cin, fileName);
	cout << endl;
	dictionary.writeFile(fileName);

	dictionary.populateString2();
	dictionary.writeToFile();

	dictionary.closeInput();
	dictionary.closeOutput();

	system("pause");
	return 0;
}


