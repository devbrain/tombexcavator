class AiPlayerClass : public PlayerClass
{
public:
	AiPlayerClass(int newPlayerNumber, int newHouse, int newColour, int newCredits, DIFFICULTYTYPE difficulty);
	~AiPlayerClass();

	void addCredits(double newCredits);
	void clearPlaceList(int num);
	void decrementStructures(int itemID, COORDTYPE* location);
	void load(FILE* fp);
	void noteDamageLocation(void* object, COORDTYPE* location);
	void save(FILE* fp);
	void setAttackTimer(int newAttackTimer);
	void setDifficulty(DIFFICULTYTYPE newDifficulty);
	void update();
	bool isAttacking();

	inline int getAttackTimer() { return attackTimer; }
	inline DIFFICULTYTYPE getDifficulty() { return difficulty; }

private:
	COORDTYPE* findPlaceLocation(int structureSizeX, int structureSizeY);

	int	attackTimer,
		buildTimer;

	double spiceMultiplyer;
	DIFFICULTYTYPE	difficulty;
	ListIterator	placeLocations;
};

