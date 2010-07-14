#include "../sand.h"
//#include "gui.h"

Box::Box() : Container()
{
	aBox = true;
}
Box::~Box()
{

}

void Box::addChild(Widget* newChild)
{
	children.insertLast(newChild);
	resizeChildren();
}

