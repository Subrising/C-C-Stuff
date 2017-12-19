#ifndef GRID_UPDATE_H
#define GRID_UPDATE_H

#include "Grid.h"

class GridUpdate
{
public:
	static void setNodesPath( Grid* Grid, GridNode * endNode );
	static bool isGridNode( GridNode node );
	static void clearMarkers( Grid* Grid );
	static char getMarker( GridNode node );
};

#endif
