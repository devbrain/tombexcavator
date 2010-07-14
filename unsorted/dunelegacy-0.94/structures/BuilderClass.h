class BuilderClass : public StructureClass
{
public:
	BuilderClass(PlayerClass* newOwner);
        ~BuilderClass();

	void setOwner(PlayerClass *no);
	void setupButtons();
	virtual void buildRandom();
	void buildUpdate();
	static void buttonUp(void* data);
	static void buttonDown(void* data);
	virtual bool buildNum(bool multipleMode, int num);
	void buildType(int itemID);
	void cancelSelection(bool multipleMode, int num);
	virtual void cancelSelection(BuildItemClass* item);
	void checkMinMaxSelection();
	virtual void checkSelectionList();	//goes and sees what should be in its list
	void doDown();
	void doUp();
	void drawItems(int minX, int minY, int maxX, int maxY, int spacing);
	static void handleClick(void* data);
	static void handleCancel(void* data);
	void insertItem(int itemID);
	void loadBuildList(FILE* fp);
	void netDoBuildCommand(bool mulitpleMode, int num);
	void netDoCancelCommand(bool mulitpleMode, int num);
	void netSetWaitingToPlace();
	void saveBuildList(FILE* fp);
	void setWaitingToPlace();
	void unSetWaitingToPlace();
	bool isOnHold(int num);
	bool isTypeWaitingToPlace(int typeID);
	bool isWaitingToPlace(int num);
	int getNumSelections();
	int getNumItemsToBuild();
	int getNumItemsToBuild(int pictureNum);
	int getMinBarSelection();
	int getMaxBarSelection();
	BuildItemClass* getPlaceItem();
	void update();
	void upgrade();
	static void upgradeCallback(void *val);

protected:
	int	minSelection,
		maxSelection;

	BuildItemClass* currentItem;
	
	ListIterator	*selectionList,
					*buildList;

	Button			up, down,
					b1, b2, b3, b4;

	double	priceModifier;//this really shouldnt be here...its only used for starport class
						  //but i dont believe the prices could be updated for tooltip text things
						  //from that class..*shrug*

	BUILD_ITEM_DATA	data1, data2, data3, data4;

	int upgradeLevels;
	int curUpgradeLev;
	int upgradeCost;
	int upgradeProgress;
	int lastUpgradeProgress;
	bool upgrading, allowedToUpgrade;
};
