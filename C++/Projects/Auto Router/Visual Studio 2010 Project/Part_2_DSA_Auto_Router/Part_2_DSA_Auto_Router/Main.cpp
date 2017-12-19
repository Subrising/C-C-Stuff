#include <iostream>
#include <string>

#include "Grid.h"
#include "AStarRouter.h"

using namespace std;

int main()
{
	string fileName;
	cout << "Please Enter Name of File (Remember to add .txt): " << endl;
	cin >> fileName;
	cout << endl;

	Grid grid;

	bool open = grid.load( fileName );
	if( open )
	{
		cout << "File Has Opened" << endl << endl;
	}
	else
	{
		cout << "Could Not Open File" << endl << endl;
		system( "pause" );
		return 0;
	}

	checkEnd result = AStarRouter::markPath( & grid );
	if( result.succeeded )
	{
		cout << "You have reached your destination!" << endl << endl;
		cout << grid << endl;
		cout << result << endl;
	}
	else
	{
		cout << "No Valid Paths Found" << endl << endl;
	}

	cout << endl;

	system( "pause" );
	return 0;
}
