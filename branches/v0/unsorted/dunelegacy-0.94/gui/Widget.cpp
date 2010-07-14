#include "../sand.h"
//#include "gui.h"

Widget::Widget()
{
	rect.x = rect.y = 0;

	aBox = anEntry = false;
	enabled = true;
	visible = false;
}
Widget::~Widget()
{
	if (focusedWidget == this)
		focusedWidget = NULL;
}

void Widget::handleCancelPress(Uint32 x, Uint32 y)
{
	if (focusedWidget != NULL)
		focusedWidget->setFocused(false);
}

void Widget::handleCancelRelease(Uint32 x, Uint32 y)
{
	if (focusedWidget != NULL)
		focusedWidget->setFocused(false);
}

void Widget::handlePress(Uint32 x, Uint32 y)
{
	if (focusedWidget != NULL)
		focusedWidget->setFocused(false);
}

void Widget::setFocused(bool truth)
{
	if (truth)
		focusedWidget = this;
	else
		focusedWidget = NULL;
}

void Widget::setWidth(Uint32 newWidth)
{
	if (newWidth >= 0)
		rect.w = newWidth;
}

void Widget::setHeight(Uint32 newHeight)
{
	if (newHeight >= 0)
		rect.h = newHeight;
}

