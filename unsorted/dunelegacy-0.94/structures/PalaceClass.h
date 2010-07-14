class PalaceClass : public StructureClass
{
public:
	PalaceClass(PlayerClass* newOwner);
	~PalaceClass();

	void doSpecial();
	void enableSpecial(bool status);
	static void netDoSpecial(void* voidPalace);
	void doSpecificStuff();
	void setDeathhandOff(int newOffX, int newOffY);
	void setSpecialTimer(int time);
	bool callFremen();
	bool launchDeathhand();
	bool spawnSaboteur();
	void drawButton(int x, int y);
	double getPercentComplete();

	inline bool specialReady() { return (specialTimer == 0); }
	inline int getDeathOffX() { return deathOffX; }
	inline int getDeathOffY() { return deathOffY; }
	inline int getSpecialTimer() { return specialTimer; }

private:
	int		house,
			deathOffX, deathOffY,
			specialTimer;
	int picID;
	Button	palaceSpecial;
};

