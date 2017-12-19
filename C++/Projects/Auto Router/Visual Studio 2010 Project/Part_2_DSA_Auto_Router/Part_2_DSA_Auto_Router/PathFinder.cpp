#include "PathFinder.h"

#include <algorithm>
#include <math.h>

//Sorts a list of GridNode pointers by their distanceToEndNode.
struct sort_open_nodes
{
	bool operator ()( GridNode * const& a,  GridNode * const& b )
	{
		return a->distanceToEndNode < b->distanceToEndNode;
	}
};

//Initializes the Pathfinder object by setting the openList/closedList, and grid.
PathFinder::PathFinder( Grid* Grid )
{
	grid = Grid;

	openList = list<GridNode *>();
	closedList = list<GridNode *>();

	g_endNode = NULL;
}

PathFinder::~PathFinder()
{
	grid = NULL;
}

bool PathFinder::findRoute( GridNode * startNode, GridNode * endNode )
{
	if( startNode == NULL || endNode == NULL )
	{
		return false;
	}

	//Prepare Grid for path finding.
	for( int nodeIndex = 0; nodeIndex < grid->g_nodeCount; nodeIndex++ )
	{
		GridNode * node = grid->g_nodes + nodeIndex;

		int dx = abs( node->x - endNode->x );
		int dy = abs( node->y - endNode->y );

		//Euclidean Distance Sort
		node->distanceToEndNode = sqrt(double(dx * dx + dy * dy ));
		node->parent = NULL;
	}

	//Reset node lists.
	openList.clear();
	closedList.clear();

	//Set path end node.
	g_endNode = endNode;

	//Checks the start node.
	closedList.push_back( startNode );
	addOpenNodes( startNode );

	//Checks the next node.
	GridNode * nextNode = openList.front();
	openList.pop_front();

	return checkNode( nextNode );
}

bool PathFinder::checkNode( GridNode * node )
{
	//Checks if we have reached the end node.
	if( node == g_endNode )
	{
		return true;
	}

	// Mark node as closed (checked), and adds surrounding nodes.
	closedList.push_back( node );
	addOpenNodes( node );

	//If the openList is empty, we have failed to find a path.
	if( openList.empty() )
		return false;
	else
	{
		//Checks the next node.
		GridNode * nextNode = openList.front();
		openList.pop_front();

		return checkNode( nextNode );
	}
}


void PathFinder::addOpenNodes( GridNode * node )
{
	int x = node->x;
	int y = node->y;
	int width = grid->g_width;

	//Gets node above the current node and passes it to addOpenNode to see if it is closed or open
	int topIndex = ( y - 1 ) * width + x;
	GridNode * topNode = grid->g_nodes + topIndex;
	addOpenNode( topNode, node );

	//Gets node underneath the current node and passes it to addOpenNode to see if it is closed or open
	int bottomIndex = ( y + 1 ) * width + x;
	GridNode * bottomNode = grid->g_nodes + bottomIndex;
	addOpenNode( bottomNode, node );

	//Gets node to the left of the current node and passes it to addOpenNode to see if it is closed or open
	int leftIndex = y * width + x - 1;
	GridNode * leftNode = grid->g_nodes + leftIndex;
	addOpenNode( leftNode, node );

	//Gets node to the right of the current node and passes it to addOpenNode to see if it is closed or open
	int rightIndex = y * width + x + 1;
	GridNode * rightNode = grid->g_nodes + rightIndex;
	addOpenNode( rightNode, node );

	if( topNode->value == ' ' )
	{
		//Gets node above and to the left of the current node and passes it to addOpenNode to see if it is closed or open
		if( leftNode->value == ' ' )
		{
			int topLeftIndex = ( y - 1 ) * width + x - 1;
			GridNode * topLeftNode = grid->g_nodes + topLeftIndex;
			addOpenNode( topLeftNode, node );
		}
		//Gets node above and to the right of the current node and passes it to addOpenNode to see if it is closed or open
		if( rightNode->value == ' ' )
		{
			int topRightIndex = ( y - 1 ) * width + x + 1;
			GridNode * topRightNode = grid->g_nodes + topRightIndex;
			addOpenNode( topRightNode, node );
		}
	}

	//Gets node beneath and to the left of the current node and passes it to addOpenNode to see if it is closed or open
	if( bottomNode->value == ' ' && leftNode->value == ' ' )
	{
		int bottomLeftIndex = ( y + 1 ) * width + x - 1;
		GridNode * bottomLeftNode = grid->g_nodes + bottomLeftIndex;
		addOpenNode( bottomLeftNode, node );
	}

	//Gets node beneath and to the right of the current node and passes it to addOpenNode to see if it is closed or open
	if( bottomNode->value == ' ' && rightNode->value == ' ' )
	{
		int bottomRightIndex = ( y + 1 ) * width + x + 1;
		GridNode * bottomRightNode = grid->g_nodes + bottomRightIndex;
		addOpenNode( bottomRightNode, node );
	}

	//Sort the open node list by their distance to the end node.
	openList.sort( sort_open_nodes() );
}


void PathFinder::addOpenNode( GridNode * node, GridNode * parent )
{
	bool isNodeOpen = node->value == ' ' || node->value == g_endNode->value;

	if( isNodeOpen && !isNodeClosed( node ) )
	{
		if( node->parent == NULL )
		{
			//Any node without a parent is yet to be checked.
			openList.push_back( node );

			node->parent = parent;
		}
	}
}


//Checks to see if the node is within the closedList
bool PathFinder::isNodeClosed( GridNode * node )
{
	return find( closedList.begin(), closedList.end(), node ) != closedList.end();
}
