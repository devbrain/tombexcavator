#include "sand.h"

void selectAll(ListIterator* aList)
{
        if (!aList->isEmpty())
        {
                ObjectClass *tempObject;
                
                aList->reset();
                while(aList->currentNotNull())
                {
                        tempObject = static_cast<ObjectClass*>(aList->getCurrent());
                        tempObject->setSelected(true);
                        aList->nextLink();
                }
        }
}

void unselectAll(ListIterator* aList)
{
        if (!aList->isEmpty())
        {
                ObjectClass *tempObject;
                
                aList->reset();
                while(aList->currentNotNull())
                {
                        tempObject = static_cast<ObjectClass*>(aList->getCurrent());
						tempObject->setSelected(false);
                        aList->nextLink();
                }
        }
}

void copyList(ListIterator* from, ListIterator* to)
{
//	to->clearList();

        from->reset();
        while(from->currentNotNull())
        {
	        to->insertLast(static_cast<ObjectClass*>(from->getCurrent()));
                from->nextLink();
        }
}

