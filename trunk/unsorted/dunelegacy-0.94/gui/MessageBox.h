#ifndef DUNE_MESSAGEBOX_H
#define DUNE_MESSAGEBOX_H

#pragma warning(disable:4786)

#include <queue>

class MessageBox
{
public:
	MessageBox();
	virtual ~MessageBox(){return;};

	void addMessage(std::string const& msg); 
	void addUrgentMessage(std::string const& msg);
	void draw();
	void initialize();
	void setX(int x){myButton.setX(x);};
	void setY(int y){myButton.setY(y);};
	
	int getX(){return myButton.getX();};
	int getY(){return myButton.getY();};
	int getHeight(){return myButton.getHeight();};
	int getWidth(){return myButton.getWidth();};
private:
	
	
	//data members
	Button myButton;
	std::queue<std::string> Messages;	
	bool setup;
	int timer;
};

#endif //#define DUNE_MESSAGEBOX_H

