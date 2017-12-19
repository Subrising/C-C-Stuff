#ifndef H_msTree
#define H_msTree

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <string.h>
#include <algorithm>
#include "graphType.h"
#include <time.h>

using namespace std;

// Node class used for vector priority queue in Prim's 2 Algorithm
class node
{
public:
	node()
	{
		nodeVertex = NULL;
		parentNode = NULL;
		edgeWeight = NULL;
	}

	int nodeVertex;
	int parentNode;
	int edgeWeight;
};

template<class vType, int size>
class msTreeType: public graphType<vType, size>
{
public:
	// Sorting method that is used to sort node objects by their edgeWeight value
	struct sort_open_vertices
	{
		bool operator ()( node const& a,  node const& b )
		{
			return a.edgeWeight < b.edgeWeight;
		}
	};

	void createSpanningGraph();
	//Function to create the graph and the weight matrix.

	void prim1(vType sVertex);
	//Function to create the edges of the minimal
	//spanning tree. The weight of the edges is also
	//saved in the array edgeWeights.

	void setNode(vector<node> &currentNodes, int index, int parent, int edgeWeight);

	void prim2(vType sVertex);

	void printTreeAndWeight();
	//Function to output the edges and the weight of the 
	//minimal spanning tree.

protected:
	// Attributes
	vType source;
	double weights[size][size];
	int edges[size];
	double edgeWeights[size];
	fstream myFile;
	bool visited[size];
};

template <class vType, int size>
void msTreeType<vType, size>::createSpanningGraph()
{   
	int selection;

	cout <<"Enter a positive number for randomly generated graph of that size" << endl << endl;
	cout <<"Enter (0) for the Textbook example graph (figure 12-16)" << endl << endl;
	cout <<"Enter a negative number to read in graph.txt" << endl << endl;
	cout <<"Enter your desired option: ";

	cin >> selection;
	cout << endl;

	srand(time(0));

	// If the selection input is a positive number, a graph will be created
	// of input * input size.

	// This section goes through and randomly generates the weight connections
	// between vertices. Because this is a mirrored graph, the random calculation
	// for the weight connections only needs to be performed for half of the grid
	// as the other half will use the same values.
	if (selection > 0)
	{  
		gSize = selection;
		for (int i = 0; i < gSize; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				if(i == j)
				{
					weights[i][j] = infinity;
					graph[i].insertLast(infinity);
				}
				else
				{
					weights[i][j] = rand() % 9 + 1;
					weights[j][i] = weights[i][j];
					graph[i].insertLast(j);
					graph[j].insertLast(i);
				}
			}
		}
	}

	// If the selection input is 0, a graph will be generated from the data 
	// in the textbookGraph.txt file. 

	// This section goes through the text file character by character and line
	// by line to correctly initialize the graph to the specifications in the 
	// text file.
	if(selection == 0)
	{   
		gSize = 7;
		int row = 0;
		int column = 0;
		string count = "";
		char c;
		myFile.open("textbookGraph.txt", fstream::in);
		if (!myFile.is_open())
		{
			cout << "Could Not Open File!\n" << endl;
			exit(1);
		}
		else
		{
			while((c = myFile.get()) != EOF)
			{   
				if(c != ' ')
				{   
					if(c == '\n')
					{   
						if(row == column)
						{
							weights[row][column] = infinity;
							graph[row].insertLast(infinity);
						}
						else
						{   
							if(atof(count.c_str()) == 0)
								weights[row][column] = infinity;
							else
							{
								weights[row][column] = atof(count.c_str());
								graph[row].insertLast(column);
							}
						}
						row++;
						column = 0;
						count = "";	
					}
					else
					{
						count += c;
					}
				}
				else
				{   
					if(row == column)
					{
						weights[row][column] = infinity;
						graph[row].insertLast(infinity);
					}
					else
					{
						if(atof(count.c_str()) == 0)
							weights[row][column] = infinity;
						else
						{
							weights[row][column] = atof(count.c_str());
							graph[row].insertLast(column);
						}
					}
					column++;
					count = "";
				}	
			}
		}
		myFile.close();
	}

	// If the selection input is 0, a graph will be generated from the data 
	// in the graph.txt file. 

	// This section goes through the text file character by character and line
	// by line to correctly initialize the graph to the specifications in the 
	// text file.
	if(selection < 0)
	{  
		gSize = 10;
		int row = 0;
		int column = 0;
		string count = "";
		char c;
		myFile.open("graph.txt", fstream::in);
		if (!myFile.is_open())
		{
			cout << "Could Not Open File!\n" << endl;
			exit(1);
		}
		else
		{
			while((c = myFile.get()) != EOF)
			{   
				if(c != ' ')
				{   
					if(c == '\n')
					{   
						if(row == column)
						{
							weights[row][column] = infinity;
							graph[row].insertLast(infinity);
						}
						else
						{   
							if(atof(count.c_str()) == 0)
								weights[row][column] = infinity;
							else
							{
								weights[row][column] = atof(count.c_str());
								graph[row].insertLast(column);
							}
						}
						row++;
						column = 0;
						count = "";	
					}
					else
					{
						count += c;
					}
				}
				else
				{   
					if(row == column)
					{
						weights[row][column] = infinity;
						graph[row].insertLast(infinity);
					}
					else
					{
						if(atof(count.c_str()) == 0)
							weights[row][column] = infinity;
						else
						{
							weights[row][column] = atof(count.c_str());
							graph[row].insertLast(column);
						}
					}
					column++;
					count = "";
				}	
			}
		}
		myFile.close();
	}
} 

// This method sets the values of a node object.
// It checks for the node index first to find the node that needs updating
// and then the values of that node are updated and the method returns
template <class vType, int size>
void msTreeType<vType, size>::setNode(vector<node> &currentNodes, int index, int parent, int edgeWeight)
{
	for (int i = 0; i < currentNodes.size(); i++)
		if (currentNodes[i].nodeVertex == index)
		{
			currentNodes[i].parentNode = parent;
			currentNodes[i].edgeWeight = edgeWeight;
			return;
		}
		return;
}

template<class vType, int size>
void msTreeType<vType, size>::prim2(vType sVertex)
{
	vType currentVertex;
	source = sVertex;
	currentVertex = sVertex;

	for(int n = 0; n < gSize; n++)
	{
		visited[n] = false;
	}

	// Initializes vector priority queue
	vector<node> nodeValues;

	for (int i = 0; i < gSize; i++)
	{
		if (i != source)
		{
			node newNode;
			newNode.nodeVertex = i;
			edgeWeights[i] = infinity;
			newNode.edgeWeight = infinity;
			newNode.parentNode = NULL;

			nodeValues.push_back(newNode);
		}
	}

	// Sets source node values
	visited[source] = true;
	edgeWeights[source] = infinity;
	edges[source] = 0;

	// Minimum spanning tree algorithm using vector priority queue
	while (nodeValues.size() != 0)
	{
		for (int i = 0; i < gSize; i++)
			if (!visited[i])
			{
				if (weights[currentVertex][i] < edgeWeights[i])
				{
					setNode(nodeValues, i, source, weights[currentVertex][i]);

					edgeWeights[i] = weights[currentVertex][i];
					edges[i] = currentVertex;
				}
			}

			sort(nodeValues.begin(), nodeValues.end(), sort_open_vertices());

			currentVertex = nodeValues[0].nodeVertex;
			nodeValues.erase(nodeValues.begin());
			visited[currentVertex] = true;
	}
}

template<class vType, int size>
void msTreeType<vType, size>::prim1(vType sVertex)
{
	int i,j,k;
	vType startVertex, endVertex;
	double minWeight;

	source = sVertex;

	bool mstv[size];

	for(j = 0; j < gSize; j++)
	{
		mstv[j] = false;
		edges[j] = source;
		edgeWeights[j] = weights[source][j];
	}

	mstv[source] = true;
	edgeWeights[source] = 0;

	for(i = 0; i < gSize - 1; i++)
	{
		minWeight = infinity;

		for(j = 0; j < gSize; j++)
			if(mstv[j])
				for(k = 0; k < gSize; k++)
					if(!mstv[k] && weights[j][k] < minWeight)
					{
						endVertex = k;
						startVertex = j;
						minWeight = weights[j][k];
					}
					mstv[endVertex] = true;
					edges[endVertex] = startVertex;
					edgeWeights[endVertex] = minWeight;
	} 
} 

template<class vType, int size>
void msTreeType<vType, size>::printTreeAndWeight()
{
	double treeWeight = 0;

	cout << fixed << showpoint << setprecision(2);

	cout << "Source Vertex: " << source << endl;
	cout << "Edges    Weight" << endl;

	// Prints out edges and edge weights
	for (int j = 0; j < gSize; j++)
	{
		if (j != source)
			if(edges[j] != j)
			{
				treeWeight = treeWeight + edgeWeights[j];
				cout << "(" <<edges[j] << ", "<< j << ")    " << edgeWeights[j] << endl;
			}
	}

	cout << endl;

	// Prints out 2D Matrix
	for (int i = 0; i < gSize; i++)
	{
		for (int j = 0; j < gSize; j++)
			if (weights[i][j] == 10000000)
				cout << "~ ";
			else
				cout << (int)weights[i][j] << " ";
		cout << endl;
	}

	cout << endl;
	cout << "Tree Weight: " << treeWeight << endl;
} //end printTreeAndWeight


#endif