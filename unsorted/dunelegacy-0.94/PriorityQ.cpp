#include "sand.h"

PriorityQ::PriorityQ(int mx)            // constructor
{
	maxSize = mx;
	currentSize = 0;
	heapArray = new TerrainClass*[maxSize];
	for (int i = 0; i < maxSize; i++)
		heapArray[i] = NULL;
}
PriorityQ::~PriorityQ()
{
	while (!isEmpty())
		removeNode();

	delete[] heapArray;
}
void PriorityQ::trickleDown(int index)
{
	int smallerChild;
	TerrainClass* top = heapArray[index];        // save root
	while(index < currentSize/2)        // not on bottom row
	{
		int leftChild = 2*index+1;
		int rightChild = leftChild+1;
							  // find smaller child
		if(rightChild < currentSize && heapArray[leftChild]->f < heapArray[rightChild]->f)
			smallerChild = leftChild;
		else
			smallerChild = rightChild;
							  // top >= largerChild?
		if(top->f <= heapArray[smallerChild]->f)
			break;
							  // shift child up
		heapArray[index] = heapArray[smallerChild];
		index = smallerChild;             // go down
	}  // end while
	heapArray[index] = top;             // root to index
}

void PriorityQ::trickleUp(int index)
{
	int parent = (index - 1)/2;
	TerrainClass* bottom = heapArray[index];

	while (index > 0 && heapArray[parent]->f > bottom->f)
	{
		heapArray[index] = heapArray[parent];
		index = parent;
		parent = (parent - 1)/2;
	}

	heapArray[index] = bottom;
}

bool PriorityQ::insertNode(TerrainClass* newNode)
{
	if (currentSize == maxSize)
		return false;
	heapArray[currentSize] = newNode;
	trickleUp(currentSize++);

	return true;
}

bool PriorityQ::insertObject(void* newObject)
{
	if (currentSize == maxSize)
		return false;
//	TerrainClass* newNode = new TerrainClass(newObject);
//	heapArray[currentSize] = newNode;
//	trickleUp(currentSize++);

	return true;
}

TerrainClass* PriorityQ::findNodeWithKey(COORDTYPE* location)
{
	if (isEmpty())
		return NULL;
	
	int tempCurrent = 0;
	while((heapArray[tempCurrent]->location.x != location->x) || (heapArray[tempCurrent]->location.y != location->y))
	{
		tempCurrent++;
		if (heapArray[tempCurrent] == NULL)
			return NULL;
	}
	
	return heapArray[tempCurrent];
}
TerrainClass* PriorityQ::removeNode()           // delete item with max key
{                           // (assumes non-empty list)
	TerrainClass* root = heapArray[0];
	heapArray[0] = heapArray[--currentSize];
	trickleDown(0);

	return root;
}
/*
void* PriorityQ::remove()           // delete item with max key
{                           // (assumes non-empty list)
	TerrainClass* root = heapArray[0];
	heapArray[0] = heapArray[--currentSize];
	trickleDown(0);

	void* object = root->location;
	delete root;
	return object;
}
*/
TerrainClass* PriorityQ::removeNodeWithKey(COORDTYPE* location)
{
	if (isEmpty())
		return NULL;
	
	int tempCurrent = 0;
	while ((heapArray[tempCurrent]->location.x != location->x) || (heapArray[tempCurrent]->location.y != location->y))
	{
		tempCurrent++;
		if (heapArray[tempCurrent] == NULL)
			return NULL;
	}

	TerrainClass* foundNode = heapArray[tempCurrent];

	heapArray[tempCurrent] = heapArray[--currentSize];
	for (int i = currentSize/2 - 1; i >= 0; i--)
		trickleDown(i);
	
	return foundNode;
}

