class MapClass
{
public:
	MapClass(int xSize, int ySize);
	~MapClass();

	void createSandRegions();
	void damage(ObjectClass* damager, PlayerClass* damagerOwner, COORDTYPE* realPos, int itemID, int damage, int damagePiercing, int damageRadius, bool air);
	void findClosestFreeSpot(COORDTYPE* origin, COORDTYPE* location, COORDTYPE* closestPoint);
	void getMapPos(int angle, COORDTYPE* source, COORDTYPE* pos);
	void removeObjectFromMap(ObjectClass* tempObject);
	void removeSpice(int xPos, int yPos);	//fixes spice tiles after spice gone to make things look smooth
	void selectObjects(int playerNum, int x1, int y1, int x2, int y2, int realX, int realY, bool objectARGMode);
	void setWinFlags(int newWinFlags);
	void viewMap(int playerTeam, COORDTYPE* location, int maxViewRange);
	void viewMap(int playerTeam, int x, int y, int maxViewRange);
	
	bool findSpice(COORDTYPE* destination, COORDTYPE* origin);
	bool perfectlyInACell(ObjectClass* thing);
	bool okayToPlaceStructure(int x, int y, int buildingSizeX, int buildingSizeY, bool tilesRequired, PlayerClass* aPlayer);
	bool isWithinBuildRange(int x, int y, PlayerClass* aPlayer);
	bool okayToBuildExclusive(int x, int y, int buildingSizeX, int buildingSizeY);
	int getPosAngle(COORDTYPE* source, COORDTYPE* pos);
	BLOCKEDTYPE cellBlocked(int xPos, int yPos);
	COORDTYPE findClosestEdgePoint(COORDTYPE* origin, COORDTYPE* buildingSize);
	COORDTYPE findDeploySpot(COORDTYPE* origin, COORDTYPE* gatherPoint, COORDTYPE* buildingSize, ObjectClass* object);//building size is num squares
	ObjectClass* findObjectWidthID(int objectID, int lx, int ly);
	

	inline bool cellExists(int xPos, int yPos) { return ((xPos >= 0) && (xPos < sizeX) && (yPos >= 0) && (yPos < sizeY)); }
	inline bool cellExists(COORDTYPE* pos) { return cellExists(pos->x, pos->y); }
	inline int getWinFlags() { return (int)winFlags; }
	inline TerrainClass* getCell(int xPos, int yPos) { return &cell[xPos][yPos]; }
	inline TerrainClass* getCell(COORDTYPE* location) { return getCell(location->x, location->y); }

	short	sizeX, sizeY, winFlags,
			*depthCheckCount,
			***depthCheckMax;
	
	TerrainClass **cell;

private:

	ObjectClass* lastSinglySelectedObject;
};

