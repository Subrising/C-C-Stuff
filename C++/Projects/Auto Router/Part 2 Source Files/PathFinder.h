#ifndef ROUTE_FINDER_H
#define ROUTE_FINDER_H

#include <list>

#include "Grid.h"

class PathFinder
{
public:
	PathFinder( Grid* Grid );
	~PathFinder();
	bool findRoute( GridNode * startNode, GridNode * endNode );
	bool checkNode( GridNode * node );
	void addOpenNodes( GridNode * node );
	bool isNodeClosed( GridNode * node );
	void addOpenNode( GridNode * node, GridNode * parent );

	Grid * grid;
	list<GridNode *> openList;
	list<GridNode *> closedList;
	GridNode * g_endNode;
};

#endif
