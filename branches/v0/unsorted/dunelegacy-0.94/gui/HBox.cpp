#include "../sand.h"
//#include "gui.h"

HBox::HBox() : Box()
{
}
HBox::~HBox()
{

}

void HBox::resizeChildren()
{
	if (!children.isEmpty())
	{
		Uint16	count = 0,
				numWidgets = children.getNumElements(),
				widgetHeight = rect.h,
				widgetWidth = (rect.w - ((numWidgets - 1)*buffering))/numWidgets;

		Widget* widget;

		children.saveCurrent();
		children.reset();
		while(children.currentNotNull())
		{
			widget = (Widget*)children.getCurrent();
			widget->setX(rect.x + ((widgetWidth + buffering)*count));
			if (children.atEnd())
				widget->setWidth(rect.w - count*(widgetWidth + buffering));
			else
				widget->setWidth(widgetWidth);
			widget->setY(rect.y);
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

