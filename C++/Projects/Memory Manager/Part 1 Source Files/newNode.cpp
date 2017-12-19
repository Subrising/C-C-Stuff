#include "newNode.h"

newNode::newNode(int l, newNode * nextNode, char * data)
{
	length = l;
	next = nextNode;
	memPt = data;
}

newNode::newNode(const newNode & n)
{
	length = n.length;
	next = n.next;
}

newNode & newNode::operator = (const newNode & n)
{
	length = n.length;
	next = n.next;
	return *this;
}
