#include "sand.h"

LinkedList::LinkedList()
{
	first = NULL;
	last = NULL;
}
LinkedList::~LinkedList()
{
	clearList();
}

void LinkedList::clearList()
{
	while(!isEmpty())
		removeFirst();
}

void LinkedList::insertFirst(void* newData)  // insert first
{
	Link* newLink = new Link(newData);

	if (isEmpty())
		last = newLink;
//	else
//		first->previous = newLink;

	newLink->next = first;
	first = newLink;
}
void LinkedList::insertLast(void* newData)  // insert last
{
	Link* newLink = new Link(newData);

	if (isEmpty())
		first = newLink;
	else
	{
		last->next = newLink;
//		newLink->previous = last;
	}

	last = newLink;
}
void* LinkedList::removeFirst()
{
	//if (isEmpty())
	//	return NULL;
	
	Link* oldFirst =  first;

	first = first->next;
	//first->previous = NULL;

	void* oldData = oldFirst->dData;
	delete oldFirst;

	return oldData;
}

/*
void* LinkedList::getElement(int num)
{
	TerrainClass* tempCurrent = first;

	int count = 0;
	while (count != num)
	{
		tempCurrent = tempCurrent->next;
		count++;
	}

	return tempCurrent->location;
}
*/

/*
TerrainClass* LinkedList::findNodeWithKey(COORDTYPE* location)
{
	if (isEmpty())
		return NULL;
	
	TerrainClass* tempCurrent = first;
	while (&tempCurrent->location != location)
	{
		tempCurrent = tempCurrent->next;
		if (tempCurrent == NULL)
			return NULL;
	}
	
	return tempCurrent;
}

TerrainClass* LinkedList::removeNodeWithKey(COORDTYPE* location)
{
	if (isEmpty())
		return NULL;
	
	TerrainClass* tempCurrent = first;
	while (&tempCurrent->location != location)
	{
		tempCurrent = tempCurrent->next;
		if (tempCurrent == NULL)
			return NULL;
	}
	
	if (tempCurrent == first)
		first = tempCurrent->next;
	else
		tempCurrent->previous->next = tempCurrent->next;

	if (tempCurrent == last)
		last = tempCurrent->previous;
	else
		tempCurrent->next->previous = tempCurrent->previous;
	
	return tempCurrent;
}
*/

/*
LinkedList::LinkedList()
{
	first = NULL;
	last = NULL;
}
void LinkedList::insertFirst(void* newObject)
{
	TerrainClass* newLink = new TerrainClass(newObject);

	if (isEmpty())
		last = newLink;
	newLink->next = first;
	first = newLink;
}
void LinkedList::insertLast(void* newObject)
{
	TerrainClass* newLink = new TerrainClass(newObject);

	if (isEmpty())
		first = newLink;
	else
		last->next = newLink;
	last = newLink;
}
void* LinkedList::deleteFirst()
{
	TerrainClass* oldLink = first;
	void* oldObject = first->location;
	if (first->next == NULL)
		last = NULL;
	first = first->next;

	delete oldLink;
	return oldObject;
}
*/

