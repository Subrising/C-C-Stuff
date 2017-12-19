#include "GridUpdate.h"

//Sets the path leading linking nodes
void GridUpdate::setNodesPath( Grid* Grid, GridNode * endNode )
{
	char marker = getMarker( *endNode );
	GridNode * gridNode = endNode->parent;
	while( gridNode->parent != NULL )
	{
		gridNode->value = marker;
		gridNode = gridNode->parent;
	}
}

//Clears current markers that were set when a path was found but not all paths could be found in this iteration.
void GridUpdate::clearMarkers( Grid* Grid )
{
	for( int nodeIndex = 0; nodeIndex < Grid->g_nodeCount; nodeIndex++ )
	{
		GridNode * node = Grid->g_nodes + nodeIndex;
		if( node->value != '#' && !isGridNode( *node ) )
		{
			node->value = ' ';
		}
	}
}

//Returns a marker value using ascii values. For example, '0' is 48. 49 + 56 is 97 which is a.
char GridUpdate::getMarker( GridNode node )
{
	return node.value + 49;
}

//Checks to see if the value of the node is either '0', '1', or '2'
bool GridUpdate::isGridNode( GridNode node )
{
	return node.value >= 48 && node.value <= 51;
}