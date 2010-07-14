#define DAMAGEPERCELL 5
#define FOGTIMEOUT 10
typedef struct

{

	int damageType, tile;

	COORDTYPE realPos;

} DAMAGETYPE;



class TerrainClass
{
public:
	TerrainClass();
	~TerrainClass();

	void assignAirUnit(ObjectClass* newObject);

	void assignDeadObject(ObjectClass* newObject);

	void assignNonInfantryGroundObject(ObjectClass* newObject);

	//void assignObject(ObjectClass* newObject);

	int assignInfantry(ObjectClass* newObject, int currentPosition);
	void assignUndergroundUnit(ObjectClass* newObject);

	void blitToScreen(int xPos, int yPos);

	void clearDamage();

	void damageCell(ObjectClass* damager, PlayerClass* damagerOwner, COORDTYPE* realPos, int bulletType, int bulletDamage, int damagePiercing, int damageRadius, bool air);

	void selectAllPlayersUnits(int playerNum, ObjectClass** lastCheckedObject, ObjectClass** lastSelectedObject);

	void selectAllPlayersUnitsOfType(int playerNum, int itemID, ObjectClass** lastCheckedObject, ObjectClass** lastSelectedObject);

	void unassignAirUnit(ObjectClass* newObject);

	void unassignDeadObject(ObjectClass* newObject);

	void unassignNonInfantryGroundObject(ObjectClass* newObject);

	void unassignObject(ObjectClass* newObject);

	void unassignInfantry(ObjectClass* newObject, int currentPosition);
	void unassignUndergroundUnit(ObjectClass* newObject);
	void setType(int newType);

	void squash();
	bool hasAnObject();
	int getInfantryTeam();
	double harvestSpice();

	ObjectClass* getAirUnit();

	ObjectClass* getDeadObject();

	ObjectClass* getNonInfantryGroundObject();

	ObjectClass* getUndergroundUnit();

	ObjectClass* getGroundObject();
	ObjectClass* getInfantry();

	//ObjectClass* getInfantry(int i);

	ObjectClass* getObject();

	ObjectClass* getObjectAt(int x, int y);

	ObjectClass* getObjectWithID(int objectID);

	//ObjectClass* selectAll();

	inline void setExplored(int player, bool truth) { if(truth)
														lastAccess[player] = clock();
													  explored[player] = truth; }
	inline void setHideTile(int newTile) { hideTile = newTile; }
	
	inline void setFogTile(int newTile) { fogTile = newTile; }
	
	inline void setOwner(int newOwner) { owner = newOwner; }

	inline void setSandRegion(int newSandRegion) { sandRegion = newSandRegion; }
	inline void setTile(int newTile) { tile = newTile; }

	inline bool hasADeadObject() { return !assignedDeadObjects.isEmpty(); }

	inline bool hasAGroundObject() { return (hasInfantry() || hasANonInfantryGroundObject()); }

	inline bool hasAnAirUnit() { return !assignedAirUnits.isEmpty(); }

	inline bool hasAnUndergroundUnit() { return !assignedUndergroundUnits.isEmpty(); }

	inline bool hasANonInfantryGroundObject() { return !assignedNonInfantryGroundObjects.isEmpty(); }

	inline bool hasInfantry() { return !assignedInfantry.isEmpty(); }
	inline bool hasSpice() { return (fixDouble(spice) > 0.0); }
	inline bool infantryNotFull() { return (assignedInfantry.getNumElements() < NUM_INFANTRY_PER_CELL); }
	inline bool isConcrete() { return (type == Structure_Slab1); }
	inline bool isExplored(int player) 	{return explored[player];}
	
	inline bool isFogged(int player);
	inline bool isNextToHidden() { return (hideTile > 0); }
	inline bool isNextToFogged() { return (fogTile > 0); }
	inline bool isMountain() { return (type == Terrain_Mountain);}
	inline bool isRock() { return ((type == Terrain_Rock) || (type == Structure_Slab1) || (type == Terrain_Mountain));}

	inline bool isSand() { return ((type == Terrain_Dunes) || (type == Terrain_Sand)); }
	inline bool isBloom() { return ((type == Terrain_Sand) && ((tile == Terrain_a2) || (tile == Terrain_a3))); }
	inline bool isSpice() { return ((type == Terrain_Spice) || (type == Terrain_ThickSpice)); }
	inline bool isThickSpice() { return (type == Terrain_ThickSpice); }
	inline int getHideTile() { return hideTile; }
	inline int getFogTile() { return fogTile; }
	
	inline int getSandRegion() { return sandRegion; }

	inline int getDamageType() { return damageType; }
	inline int getOwner() { return owner; }
	inline int getType() { return type; }
	inline int getTile() { return tile; }
	inline double getDifficulty() { return difficulty;} 
	Uint32 getRadarColour(int Player, bool radar);
	inline double getSpiceRemaining() { return fixDouble(spice); }
	

	inline COORDTYPE* getLocation() { return &location; }

	inline ListIterator* getAirUnitList() { return &assignedAirUnits; }

	inline SDL_Surface* getSprite() { return sprite; }

	bool	visited;
						/* for path searching */
	double	cost,		//cost to get here from original location
			f,			//cost + heuristic
			heuristic;	//estimate for how much it will cost to get from here to dest

	COORDTYPE	location;

	TerrainClass	*parent,
					*previous,
					*next;
private:
	bool	*explored;
			
	clock_t	*lastAccess;
	int	damagePos,

		damageType,

		hideTile,
		fogTile,
		fogColour,/*remember last colour(radar)*/
		owner,
		sandRegion,	//used by sandworms to check if can get to a unit
		tile, //need pretile because when smoothing random map, you need to know what all the tiles are before smoothed
		type;

	double	difficulty,
			spice;		//how much spice on this particular cell is left


	DAMAGETYPE		damage[DAMAGEPERCELL];		//damage positions


	ListIterator	assignedAirUnits,
					assignedDeadObjects,

					assignedNonInfantryGroundObjects,

					assignedUndergroundUnits,
					assignedInfantry;
	
	SDL_Surface		*sprite;
			

};

