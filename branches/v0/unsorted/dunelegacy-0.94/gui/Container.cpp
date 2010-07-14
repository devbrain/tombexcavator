#include "../sand.h"
//#include "gui.h"

Container::Container() : Widget()
{
	buffering = 10;
	rect.w = rect.h = 0;
}
Container::~Container()
{
	/*if (!children.isEmpty())
	{
		children.reset();
		while(children.currentNotNull())
			delete ((Widget*)children.removeCurrent());
	}*/
}

void Container::addChild(Widget* newChild)
{
	children.insertLast(newChild);
}

void Container::draw()
{
	if (visible)
	{
		if (!children.isEmpty())
		{
			children.reset();
			while(children.currentNotNull())
			{
				((Widget*)children.getCurrent())->draw();
				children.nextLink();
			}
		}
	}
}

void Container::dropChildren()
{
	children.clearList();
}

void Container::handleMotion(Uint32 x, Uint32 y)
{
	Widget* widget;
	children.reset();
	while(children.currentNotNull())
	{
		widget = (Widget*)children.getCurrent();
		widget->handleMotion(x, y);
		children.nextLink();
	}
}

void Container::handleCancelPress(Uint32 x, Uint32 y)
{
    //if over widget area
	if (enabled && !children.isEmpty()
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{	//find the widget mouse over

		bool found = false;
		Widget* widget;

		children.reset();
		while(!found && children.currentNotNull())
		{
			widget = (Widget*)children.getCurrent();
			widget->handleCancelPress(x, y);
			children.nextLink();
		}
	}
}

void Container::handleCancelRelease(Uint32 x, Uint32 y)
{
    //if over widget area
	if (enabled && !children.isEmpty()
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{	//find the widget mouse over

		bool found = false;
		Widget* widget;

		children.reset();
		while(!found && children.currentNotNull())
		{
			widget = (Widget*)children.getCurrent();
			widget->handleCancelRelease(x, y);
			children.nextLink();
		}
	}
}

void Container::handlePress(Uint32 x, Uint32 y)
{
    //if over widget area
	if (enabled && !children.isEmpty()
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{	//find the widget mouse over

		bool found = false;
		Widget* widget;

		children.reset();
		while(!found && children.currentNotNull())
		{
			widget = (Widget*)children.getCurrent();
			widget->handlePress(x, y);
			children.nextLink();
		}
	}
}

void Container::handleRelease(Uint32 x, Uint32 y)
{
	//if over widget area
	if (!children.isEmpty()
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{	//find the widget mouse over

		bool found = false;
		Widget* widget;

		children.reset();
		while(!found && children.currentNotNull())
		{
			widget = (Widget*)children.getCurrent();
			widget->handleRelease(x, y);
			children.nextLink();
		}
	}
}

void Container::setBuffering(int newBuffering)
{
	if (newBuffering >= 0)
		buffering = newBuffering;
}

