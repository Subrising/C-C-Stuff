
#ifndef H_graph
#define H_graph

#include <iostream>
#include <fstream>
#include <iomanip>
#include "linkedList.h"
#include "linkedListForGraph.h"
#include "queuelinked.h"

using namespace std;

const int infinity = 10000000;   //This will be used in later 
                                 //sections of this chapter, when 
                                 //we discuss weighted graphs.

template<class vType, int size>
class graphType
{
public:
    bool isEmpty();
      //Function to determine whether the graph is empty.
      //Postcondition: Returns true if the graph is empty; 
      //               otherwise, returns false.
    void createGraph();
      //Function to create the graph using the adjacency list 
      //representation.
      //Postcondition: The graph is created in the form of
      //               adjacenty lists.
    void clearGraph();
      //Function to deallocate the memory occupied by the linked 
      //lists and the array of pointers pointing to the linked
      //lists.
    void printGraph() const;
      //Function to print the graph.

    void depthFirstTraversal();
      //Function to perform the depth first traversal of
      //the entire graph.
    void dftAtVertex(vType vertex);
      //Function to perform the depth first traversal of 
      //the graph at a node specified by the parameter vertex.

    void breadthFirstTraversal();
      //Function to perform the breadth first traversal of
      //the entire graph.

    graphType(); 
      //default constructor
      //Postcondition: The graph size is set to 0, that is,
      //               gSize = 0; maxSize = size.

    ~graphType();
      //destructor
      //Postcondition: The storage occupied by the graph 
       //               is deallocated.

protected:
    int maxSize;    //maximum number of vertices
    int gSize;      //current number of vertices
    linkedListGraph<vType> graph[size]; //array of pointers
                //to create the adjacency lists (linked lists)

private:
    void dft(vType v, bool visited[]);
      //Function to perform the depth first traversal of 
      //the graph at a particular node. 
};


template <class vType, int size>		
bool graphType<vType, size>::isEmpty()
{
	return (gSize == 0);
}

template<class vType, int size>
void graphType<vType, size>::createGraph()
{
     ifstream infile;
     char fileName[50];

     vType vertex;
     vType adjacentVertex;

     if(gSize != 0)  //if the graph is not empty, make it empty
        clearGraph();

     cout<<"Enter the input file name: ";
     cin>>fileName;
     cout<<endl;

     infile.open(fileName);

     if(!infile)
     {
        cerr<<"Cannot open the input file."<<endl;
        return;
     }

     infile>>gSize;  //get the number of vertices

     for (int index = 0; index < gSize; index++)
     {
         infile>>vertex;
         infile>>adjacentVertex;

         while(adjacentVertex != -999)
         {
             graph[vertex].insertLast(adjacentVertex);
             infile>>adjacentVertex;
         }//end while
     }//end for

     infile.close();
}//end createGraph

template<class vType, int size>
void graphType<vType, size>::clearGraph()
{
     int index;

     for(index = 0; index < gSize; index++)
         graph[index].destroyList();

     gSize = 0;
}

template<class vType, int size>
void graphType<vType, size>::printGraph() const
{
     int index;

     for(index = 0; index < gSize; index++)
         cout<<index<<" "<<graph[index]<<endl;

     cout<<endl;
}//end printGraph


     //default constructor
template<class vType, int size>
graphType<vType, size>::graphType()  
{
     maxSize = size;
     gSize = 0;
}

     //destructor
template<class vType, int size>
graphType<vType, size>::~graphType()   
{
     clearGraph();
}

template<class vType, int size>
void graphType<vType, size>::depthFirstTraversal()
{
     bool *visited;    //array to keep track of the visited
                       //vertices
     visited = new bool[gSize];

     int index;

     for(index = 0; index < gSize; index++) 
         visited[index] = false;   
 
     for(index = 0; index < gSize; index++) //for each vertex
         if(!visited[index])          //that is not visited,
             dft(index,visited);      //do a depth first
                                     //traversal
     delete [] visited;
}//end depthFirstTraversal

template<class vType, int size>
void graphType<vType, size>::dft(vType v, bool visited[])
{
     vType w;

     vType *adjacencyList;     //array to retrieve the
                               //adjacent vertices
     adjacencyList = new vType[gSize];
 
     int alLength = 0;  //the number of adjacent vertices

     visited[v] = true;
     cout<<" "<<v<<" ";  //visit the vertex

     graph[v].getAdjacentVertices(adjacencyList, alLength);
               //retrieve the adjacent vertices into adjacencyList

     for(int index = 0; index < alLength; index++) //for each 
     {                                  //vertex adjacent to v
         w = adjacencyList[index];
         if(!visited[w])
            dft(w, visited);
     }//end for

     delete [] adjacencyList;
}//end dft

template<class vType, int size>
void graphType<vType, size>::dftAtVertex(vType vertex)
{
     bool *visited;

     visited = new bool[gSize];

     for(int index = 0; index < gSize; index++)
         visited[index] = false;
 
     dft(vertex,visited);
 
     delete [] visited;

} //end dftAtVertex



template<class vType, int size>
void graphType<vType, size>::breadthFirstTraversal()
{
   linkedQueueType<vType> queue;
   vType u;

   bool *visited;
   visited = new bool[gSize];

   for(int ind = 0; ind < gSize; ind++)
       visited[ind] = false;    //initialize the array 
                                 //visited to false

   vType *adjacencyList;
   adjacencyList = new vType[gSize];

   int alLength = 0;

   for(int index = 0; index < gSize; index++)
      if(!visited[index])
      {
         queue.addQueue(index);
         visited[index] = true;
         cout<<" "<<index<<" ";

         while(!queue.isEmptyQueue())
         {
             u = queue.front();
             queue.deleteQueue();
             graph[u].getAdjacentVertices(adjacencyList, alLength);
             for(int w = 0; w < alLength; w++)
                 if(!visited[adjacencyList[w]])
                 {
                    queue.addQueue(adjacencyList[w]);
                    visited[adjacencyList[w]] = true;
                    cout<<" "<<adjacencyList[w]<<" ";
                 } //end if
         } //end while
      } //end if
 
   delete [] visited;
   delete [] adjacencyList;
} //end breadthFirstTraversal

#endif