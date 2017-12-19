#ifndef GRID_H
#define GRID_H

#include <iostream>

using namespace std;

struct GridNode
{
	int x;
	int y;
	char value;

	int distanceToEndNode;

	GridNode * parent;

	friend ostream& operator <<( ostream& out, const GridNode node );
};

class Grid
{
	friend ostream& operator <<( ostream& out, const Grid Grid );

public:
	Grid();
	Grid( const Grid & other );
	~Grid();
	bool load( const string filename );
	const Grid& operator = ( const Grid& other );

	int g_width;
	int g_height;
	int g_nodeCount;
	GridNode * g_nodes;
};

#endif
