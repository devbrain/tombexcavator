class PlayerClass
{
public:
	PlayerClass(int newPlayerNumber, int newHouse, int newColour, int newCredits);
	virtual ~PlayerClass();
	virtual void addCredits(double newCredits);
	void assignMapPlayerNum(int newMapPlayerNum);
	void checkSelectionLists();
	void decrementUnits(int itemID);
	virtual void decrementStructures(int itemID, COORDTYPE* location);
	void freeHarvester(int xPos, int yPos);
	virtual void load(FILE* fp);
	void lose();
	void returnCredits(double newCredits);
	void sapPower(int amount);
	virtual void save(FILE* fp);
	void setStartingCredits(double newCredits);
	void setStoredCredits(double newCredits);
	void netPlaceStructure(int builderID, int x, int y, int itemID, int xPos, int yPos);
	virtual void noteDamageLocation(void* object, COORDTYPE* location);
	void unsapPower(int amount);
	virtual void update();
	void win();
	void* placeStructure(int builderID, int x, int y, int itemID, int xPos, int yPos);
	void* createUnit(int itemID);
	void* placeUnit(int itemID, int xPos, int yPos);
	int getMapPlayerNum();
	double takeCredits(double amount);
	void changeRadar(bool status);

	inline void setTeam(int newTeam) { team = newTeam; }
	inline void setQuitingGame() { quitingGame = true; }
	inline bool hasBarracks() { return (numBarracks > 0); }
	inline bool hasCarryalls() { return numCarryalls > 0; }
	inline bool hasIX() { return (numIXs > 0); }
	inline bool hasLightFactory() { return (numLightFactories > 0); }
	inline bool hasHeavyFactory() { return (numHeavyFactories > 0); }
	inline bool hasPower() { return (power >= powerRequirement); }
	inline bool hasRadar() { return (numRadars > 0); }
	inline bool hasRadarOn() { return (hasRadar() && hasPower()); }
	inline bool hasRefinery() { return (numRefineries > 0); }
	inline bool hasRepairYard() { return (numRepairYards > 0); }
	inline bool hasStarPort() { return (numStarPorts > 0); }
	inline bool hasWindTrap() { return (numWindTraps > 0); }
	inline bool isAI() { return ai; }
	inline bool isAlive() { return !(((numStructures - numWalls) <= 0) && (((numUnits - numCarryalls) <= 0))); }
	inline int getNumRadars() { return numRadars; }
	inline int getAmountOfCredits() { return (int)(credits+startingCredits); }
	inline int getCapacity() { return capacity; }
	inline int getColour() { return colour; }
	inline int getPlayerNumber() { return playerNumber; }
	inline int getPower() { return power; }
	inline int getPowerRequirement() { return powerRequirement; }
	inline int getHouse() { return house; }
	inline int getTeam() { return team; }
	inline double getStartingCredits() { return startingCredits; }
	inline double getStoredCredits() { return credits; }

protected:
	void decrementHarvesters();

	inline void addCapacity(int newCapacity) { capacity += newCapacity; }
	inline void removeCapacity(int newCapacity) { capacity -= newCapacity; }
	inline void addPower() { power += WINDTRAP_POWERSUPPLY; }
	inline void removePower() { power -= WINDTRAP_POWERSUPPLY; }

	bool		ai,
				quitingGame;

	int	playerNumber,
		mapPlayerNum,
		house,
		capacity,
		colour,
		creditsCounter,
		numBarracks,
		numCarryalls,
		numHarvesters,
		numUnits,
		numIXs,
		numHeavyFactories,
		numHighTechFactories,
		numLightFactories,
		numPalaces,
		numRefineries,
		numRepairYards,
		numRocketTurrets,
		numRadars,	//how many radars player has
		numStarPorts,
		numWalls,
		numWindTraps,
		numWORs,
		numStructures,
		power,
		powerRequirement,
		oldCredits,
		team;

	double	credits,
			startingCredits;

	char	name[MAX_LINE];
};

