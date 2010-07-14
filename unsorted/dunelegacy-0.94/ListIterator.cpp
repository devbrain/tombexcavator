#include "sand.h"

ListIterator::ListIterator()
{
	first = NULL;
	current = NULL;
	last = NULL;
	savedCurrent = NULL;
	numElements = 0;
}
ListIterator::~ListIterator()
{
	clearList();
}
void ListIterator::reset()	    // start at 'first'
{
	current = first;
}

void ListIterator::insertFirst(void* dd)  // insert first
{
	Link* newLink = new Link(dd);

	if (isEmpty())
		last = newLink;
	else
		first->previous = newLink;

	newLink->next = first;
	first = newLink;

	numElements++;
}
void ListIterator::insertLast(void* dd)  // insert last
{
	Link *newLink = new Link(dd);

	if (isEmpty())
		first = newLink;
	else
	{
		last->next = newLink;
		newLink->previous = last;
	}

	last = newLink;

	numElements++;
}

//////////remove the need for these
void ListIterator::restoreCurrent()
{
	Link*	oldLink = savedCurrent;

	savedCurrent = savedCurrent->next;
	current = (Link*)oldLink->dData;

	delete oldLink;
}

void ListIterator::saveCurrent()
{
	Link* link = new Link(current);
	link->next = savedCurrent;
	savedCurrent = link;
}

void* ListIterator::findElement(void* element)
{
	if (isEmpty())
		return NULL;
	
	Link* tempCurrent = first;
	while(tempCurrent->dData != element)
	{
		tempCurrent = tempCurrent->next;
		if (tempCurrent == NULL)
			return NULL;
	}
	
	return tempCurrent->dData;
}
void* ListIterator::removeElement(void* element)
{
	Link* tempCurrent = first;
	void* tempObject = NULL;

	if (tempCurrent)
	{
		while(tempCurrent->dData != element)
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

		if (tempCurrent == current)
			current = current->next;
		
		tempObject = tempCurrent->dData;
		delete tempCurrent;		//deletes the now free key
		numElements--;
	}

	return tempObject;
}
void* ListIterator::removeCurrent()  // delete item at current
{
	if (current == first)	// beginning of list
		first = current->next;
	else			// not beginning
		current->previous->next = current->next;

	if (current == last)
		last = current->previous;
	else
		current->next->previous = current->previous;

	Link* oldLink = current;
	void* tempObject = current->dData;
	current = current->next;
	delete oldLink;
	numElements--;

	return tempObject;
}
void* ListIterator::removeFirstElement()  // remove item at first
{
	Link* oldLink = first;

	if (first->next == NULL)
		last = NULL;
	else
		first->next->previous = NULL;

	first = first->next;

	void* tempObject = oldLink->dData;
	delete oldLink;
	numElements--;

	return tempObject;
}
void* ListIterator::getElement(int num)
{
	Link* tempCurrent = first;

	int count = 0;
	while ((count != num) && (tempCurrent != NULL))
	{
		tempCurrent = tempCurrent->next;
		count++;
	}

	if (tempCurrent == NULL)
		return NULL;
	return tempCurrent->dData;
}

void* ListIterator::getRandomElement()
{
	return (getElement(getRandomInt(0, numElements-1)));
}

void* ListIterator::getRandomElementNotCurrent()
{
	void* targetData;
	do
	{
		targetData = getRandomElement();

	} while (targetData == current->dData);

	return targetData;
}

void ListIterator::clearList()
{
	reset();
	int localNumElements = numElements;
	for (int count = 0; count < localNumElements; count++)
		removeCurrent();
}

