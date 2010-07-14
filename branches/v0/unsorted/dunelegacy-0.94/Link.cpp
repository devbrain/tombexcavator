#include "sand.h"

Link::Link(void* dd)
{
	dData = dd;
//	iData = 1;
	next = NULL;
	previous = NULL;
}

Node::Node(COORDTYPE* location, double diff)
{
	cost = 0;
	f = 0;
	heuristic = 0;
	iData = diff;
	
	dData.x = location->x;
	dData.y = location->y;
	parent = NULL;
	previous = NULL;
	next = NULL;
}

