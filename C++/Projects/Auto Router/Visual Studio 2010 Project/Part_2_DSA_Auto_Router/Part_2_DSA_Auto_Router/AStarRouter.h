#ifndef A_STAR_ROUTER_H
#define A_STAR_ROUTER_H

#include <list>
#include <iostream>

#include "Grid.h"

struct checkEnd
{
	bool succeeded;
	int linkedNodeCount;
	int isolatedNodeCount;
	int pathCount;

	friend ostream& operator << ( ostream& out, const checkEnd result );
};

class AStarRouter
{
public:
	static GridNode * closestEndNode( GridNode * startNode, list<GridNode *> nodes );
	static checkEnd markPath( Grid * Grid );
	static checkEnd findAllPaths( Grid * Grid, list<GridNode *> nodes );
	static list<GridNode *> returnNodesCount( Grid * Grid );
	static int factorial( int n );
};

#endif
