class AirUnit : public UnitClass
{
public:
	AirUnit(PlayerClass* newOwner);
	virtual ~AirUnit();

	void assignToMap(COORDTYPE* pos);
	virtual void checkPos();
	bool canPass(int xPos, int yPos);

protected:
};

