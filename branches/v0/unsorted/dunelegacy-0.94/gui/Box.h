class Box : public Container
{
public:
	Box();
	~Box();

	void addChild(Widget* newChild);
	virtual void resizeChildren() = 0;
private:
};

