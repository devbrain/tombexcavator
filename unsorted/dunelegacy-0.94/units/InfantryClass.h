 class InfantryClass : public GroundUnit
{
public:
	InfantryClass(PlayerClass* newOwner);
	~InfantryClass();

	void assignToMap(COORDTYPE* pos);
	void blitToScreen();
	virtual void checkPos();
	void destroy();
	void move();
	void setLocation(int xPos, int yPos);
	void squash();
	bool canPass(int xPos, int yPos);

	inline int getCellPosition() { return cellPosition; }
	bool canAttack(ObjectClass* object);

protected:

	void setSpeeds();

	bool squashed;

	int cellPosition,
		oldCellPosition,
		walkFrame;
};

