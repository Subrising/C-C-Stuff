#include "AStarRouter.h"

#include <algorithm>

#include "PathFinder.h"
#include "GridUpdate.h"

//Sorts the route nodes by either the x value of the value that the route node holds (0, 1, 2).
struct sort_route_nodes
{
	bool operator ()( GridNode * const& a,  GridNode * const& b )
	{
		return ( a->value == b->value ) ? a->x > b->x : a->value > b->value;
	}
};

//Prints the amount of Linked Nodes, Routes, and Isolated Nodes.
ostream& operator << ( ostream& out, const checkEnd result )
{
	out << "Linked Nodes: " << result.linkedNodeCount;
	out << "   Routes: " << result.pathCount;
	out << "   Isolated Nodes: " << result.isolatedNodeCount;

	return out;
}

checkEnd AStarRouter::markPath( Grid * Grid )
{
	checkEnd result;

	//Finds amount of nodes that need to be linked.
	list<GridNode *> amountOfNode = returnNodesCount( Grid );
	
	cout << "Found " << amountOfNode.size() << " nodes." << endl << endl;
	cout << "Searching for routes..." << endl << endl;

	if (amountOfNode.size() > 7)
		cout << "This may take awhile..." << endl;

	//Gets total amount of possible functions needed in order to determine if all paths can be found.
	int permutationCount = factorial( amountOfNode.size() );

	//Loops through process until either all paths are found or the permutation count is reached.
	for( int attemptCount = 0; attemptCount < permutationCount; attemptCount++ )
	{
		result = findAllPaths( Grid, amountOfNode );
		if( result.succeeded )
		{
			return result;
		}
		else
		{
			//Path did not go through
			GridUpdate::clearMarkers( Grid );
			next_permutation( amountOfNode.begin(), amountOfNode.end(), sort_route_nodes() );
		}
	}

	return result;
}

list<GridNode *> AStarRouter::returnNodesCount( Grid* Grid )
{
	list<GridNode *> amountOfNode;

	//Loops through to see if a nodes value is an integer.
	for( int nodeIndex = 0; nodeIndex < Grid->g_nodeCount; nodeIndex++ )
	{
		GridNode * node = Grid->g_nodes + nodeIndex;

		if( node->value != ' ' && node->value != '#' )
		{
			amountOfNode.push_back( node );
		}
	}

	//Sorts nodes by x value if they are of the same type, or by integer value if they are not.
	amountOfNode.sort( sort_route_nodes() );

	return amountOfNode;
}


checkEnd AStarRouter::findAllPaths( Grid* Grid, list<GridNode *> nodes )
{
	checkEnd result = { };
	result.isolatedNodeCount = nodes.size();

	PathFinder PathFinder( Grid );

	bool pathFound = true;

	while( !nodes.empty() && pathFound )
	{
		//Gets the start node
		GridNode * startNode = nodes.front();
		nodes.pop_front();

		//Gets the end node.
		GridNode * endNode = closestEndNode( startNode, nodes );
		if( endNode == NULL )
		{
			result.linkedNodeCount++;
			result.isolatedNodeCount--;
			continue;
		}

		//Tries to find a path.
		pathFound = PathFinder.findRoute( startNode, endNode );
		if( pathFound )
		{
			//Marks the path of the linked nodes
			GridUpdate::setNodesPath( Grid, endNode );

			//Updates the amount of nodes left, nodes linked, and the paths found.
			result.isolatedNodeCount--;
			result.linkedNodeCount++;
			result.pathCount++;
		}
	}

	result.succeeded = pathFound;
	return result;
}


GridNode * AStarRouter::closestEndNode( GridNode * startNode, list<GridNode *> nodes )
{
	GridNode * endNode = NULL;
	int endNodeDistance = 0;

	list<GridNode *>::iterator nodeIterator;
	for( nodeIterator = nodes.begin(); nodeIterator != nodes.end(); nodeIterator++ )
	{
		//Only match nodes of the same value.
		if( startNode->value != (*nodeIterator)->value )
			continue;

		//If there is no current end node, then the end node is set to the first nodeIterator.
		if( endNode == NULL )
		{
			endNode = *nodeIterator;

			int dx = abs( (*nodeIterator)->x - startNode->x );
			int dy = abs( (*nodeIterator)->y - startNode->y );
			endNodeDistance = sqrt(double(dx * dx + dy * dy ));
		}
		else
		{
			//If the end node is not null then it checks to see if this node is closer.
			int dx = abs( (*nodeIterator)->x - startNode->x );
			int dy = abs( (*nodeIterator)->y - startNode->y );
			int currentNodeDistance = sqrt(double(dx * dx + dy * dy ));

			if( currentNodeDistance < endNodeDistance )
			{
				endNode = *nodeIterator;
				endNodeDistance = currentNodeDistance;
			}
		}
	}
	return endNode;
}

int AStarRouter::factorial( int n )
{
	return ( n == 1 || n == 0 ) ? 1 : factorial( n - 1 ) * n;
}
