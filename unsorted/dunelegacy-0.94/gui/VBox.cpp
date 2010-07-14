#include "../sand.h"
//#include "gui.h"

VBox::VBox() : Box()
{
}
VBox::~VBox()
{

}

void VBox::resizeChildren()
{
	if (!children.isEmpty())
	{
		Uint16	count = 0,
				numWidgets = children.getNumElements(),
				widgetHeight = (rect.h - ((numWidgets - 1)*buffering))/numWidgets,
				widgetWidth = rect.w;

		Widget* widget;

		children.saveCurrent();
		children.reset();
		while(children.currentNotNull())
		{
			widget = (Widget*)children.getCurrent();
			widget->setX(rect.x);
			widget->setWidth(widgetWidth);
			widget->setY(rect.y + ((widgetHeight + buffering)*count));
			if (children.atEnd())
				widget->setHeight(rect.h - count*(widgetHeight + buffering));
			else
				widget->setHeight(widgetHeight);
			children.nextLink();
			count++;
		}

		children.reset();
		while(children.currentNotNull())
		{
			widget = (Widget*)children.getCurrent();
			if (widget->isABox())
				((Box*)widget)->resizeChildren();
			children.nextLink();
		}
		children.restoreCurrent();
	}
}

