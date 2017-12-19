#include "minimalSpanTreeType.h"

int main()
{   
	// Creates graph
	msTreeType<int, 100> MST;
	MST.createSpanningGraph();

	// Runs Prim's 1 Algorithm on created graph
	MST.prim1(0);
	cout << "Prim's 1 Algorithm Results:" << endl;
	MST.printTreeAndWeight();

	cout << endl;

	// Runs Prim's 2 Algorithm on created graph
	MST.prim2(0);
	cout << "Prim's 2 Algorithm Results:" << endl;
	MST.printTreeAndWeight();

	system("pause");
	return 0;
}