#include "Grid.h"
#include <fstream>
#include <iostream>

using namespace std;

ostream& operator << ( ostream& out, const GridNode node )
{
	out << "[GridNode] ";
	out << " x: " << node.x;
	out << " y: " << node.y;
	out << " value: " << node.value;

	return out;
}

Grid::Grid()
{
	g_width = 0;
	g_height = 0;
	g_nodeCount = 0;

	g_nodes = NULL;
}

Grid::Grid( const Grid & other )
{
	g_nodes = NULL;
	*this = other;
}

Grid::~Grid()
{
	if( g_nodes != NULL )
	{
		delete [] g_nodes;
		g_nodes = NULL;
	}
}

bool Grid::load( const string filename )
{
	//Removes current Grid.
	if( g_nodes != NULL )
	{
		delete [] g_nodes;
		g_nodes = NULL;
	}

	fstream newFile;
	newFile.open( filename, fstream::in );

	if( !newFile.is_open() )
		return false;

	#define ROW_BUFFER_SIZE 256

	//Sets file buffer
	char rowBuffer[ROW_BUFFER_SIZE];
	newFile.getline( rowBuffer, ROW_BUFFER_SIZE );

	if( newFile.fail() )
	{
		//Grid width is greater than our buffer size.
		return false;
	}

	g_width = newFile.gcount() - 1;

	//Reads through file and gets Grid height.
	g_height = 1;
	while( !newFile.eof() )
	{
		newFile.getline( rowBuffer, ROW_BUFFER_SIZE );

		//All rows are assumed to start with #
		if( rowBuffer[0] == '#' )
			g_height++;
	}


	//Return to start of newFile.
	newFile.close();
	newFile.open(filename);

	//Set and initialize nodes for grid.
	g_nodeCount = g_width * g_height;
	g_nodes = new GridNode[g_nodeCount];

	//Loops through g_nodes array and sets the x, y and value of the node.
	for( int yIndex = 0; yIndex < g_height; yIndex++ )
	{
		for( int xIndex = 0; xIndex < g_width; xIndex++ )
		{
			GridNode * node = g_nodes + ( yIndex * g_width ) + xIndex;
			node->x = xIndex;
			node->y = yIndex;
			node->value = newFile.get();
		}

		//Discard new line character.
		newFile.get();
	}

	newFile.close();

	return true;
}

const Grid& Grid::operator =( const Grid& other )
{
	g_width = other.g_width;
	g_height = other.g_height;
	g_nodeCount = other.g_nodeCount;

	if( g_nodes != NULL )
		delete [] g_nodes;

	g_nodes = new GridNode[g_nodeCount];

	for( int nodeIndex = 0; nodeIndex < g_nodeCount; nodeIndex++ )
		g_nodes[nodeIndex] = other.g_nodes[nodeIndex];

	return *this;
}


ostream& operator << ( ostream& out, const Grid Grid )
{
	if( Grid.g_nodes == NULL )
		return out;

	//Prints out the grid.
	for( int yIndex = 0; yIndex < Grid.g_height; yIndex++ )
	{
		for( int xIndex = 0; xIndex < Grid.g_width; xIndex++ )
			out << Grid.g_nodes[yIndex * Grid.g_width + xIndex].value;

		out << endl;
	}

	return out;
}
