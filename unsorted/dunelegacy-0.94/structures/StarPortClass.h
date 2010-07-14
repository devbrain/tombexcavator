class StarPortClass : public BuilderClass
{
public:
	StarPortClass(PlayerClass* newOwner);
	~StarPortClass();

	bool buildNum(bool multipleMode, int num);
	void buildRandom();
	void cancelSelection(BuildItemClass* item);
//	void checkMinMaxSelection();
	void checkSelectionList();
	void doSpecificStuff();
	void setArrivalTimer(int newArrivalTimer);

	void placeOrder();	//wrapper function, because can't use callbacks on classes
	void cancelOrder();

	static void netPlaceOrder(void* object);	//static because can't reference to uncreated object handler
	static void netCancelOrder(void* object);

	inline bool okToOrder() { return (arrivalTimer <= 0); }
	inline int getArrivalTimer() { return arrivalTimer; }

private:
	int		arrivalTimer;
	int		modifierTimer;

	Button	order, cancel;
	SDL_Surface *orderSurf, *orderSurfPressed, *cancelSurf, *cancelSurfPressed;
};

