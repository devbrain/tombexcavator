typedef struct
{
	void	*object;
	int		buildNum;
} BUILD_ITEM_DATA;

class BuildItemClass
{
public:
	BuildItemClass();
	BuildItemClass(int newItemID);
	~BuildItemClass();
	void buildUpdate(double credits);
	void decrementNumToBuild();
	void draw(int x, int y);
	void setWaitingToPlace(bool value);
	double getPercentComplete();
//	int getCost();

	inline void incrementNumToBuild() { numToBuild++; }
	inline void setCost(int newCost) { cost = newCost; }
	inline void setGraphic(SDL_Surface* newGraphic) { graphic = newGraphic; }
	inline void setItemID(int newItemID) { itemID = newItemID; }
	inline void setOnHold(bool value) { onHold = value; }
	inline void setNumToBuild(short newNumToBuild) { numToBuild = newNumToBuild; }
	inline void setProgress(int newProgress) { progress = newProgress; }
	inline void setStructureSize(COORDTYPE* coord) { structureSize.x = coord->x; structureSize.y = coord->y; }
	inline void unsetWaitingToPlace() { waitingToPlace = false; }
	inline bool isWaitingToPlace() { return waitingToPlace; }
	inline bool isOnHold() { return onHold; }
	inline short getStructureSizeX() { return structureSize.x; }
	inline short getStructureSizeY() { return structureSize.y; }
	inline char* getDescription() { return description; }
	inline int getItemID() { return itemID; }
	inline int getBListPriority() { return bListPriority; }
	inline int getNumToBeBuilt() { return numToBuild; }
	inline double getCost() { return cost; }
	inline char* getName() { return name; }
	inline double getProgress() { return progress; }
	inline COORDTYPE* getStructureSize() { return &structureSize; }
	inline SDL_Surface* getGraphic() { return graphic; }

protected:
	bool	waitingToPlace,
			onHold;
		
	int	itemID,
		bListPriority,
		numToBuild;

	char	description[MAX_LINE];
	char	name[MAX_LINE];

	double	cost,
			progress;

	COORDTYPE	structureSize;
		
	SDL_Surface	*graphic,
				*graphicBuffer;
};

