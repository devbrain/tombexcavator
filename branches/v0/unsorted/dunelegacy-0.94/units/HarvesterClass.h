class HarvesterClass : public TrackedUnit
{
public:
	HarvesterClass(PlayerClass* newOwner);
	~HarvesterClass();
	void blitToScreen();
	void checkPos();
	void deploy(COORDTYPE* newLocation);
	void destroy();
	virtual void drawSelectionBox();
	void handleDamage(int damage, ObjectClass* damager);
	void harvesterReturn();
	void move();
	void setAmountOfSpice(double newSpice);
	void setReturned();
	void setDestination(int newX, int newY);
	void setTarget(ObjectClass* newTarget);
	bool canAttack(ObjectClass* object);
	double extractSpice();

	inline void setHarvesting() { harvesting = true; }
	inline double getAmountOfSpice() { return spice; }
	inline bool isReturning() { return returningToRefinery; }

private:

	bool	harvesting,
			returningToRefinery;
	double	spice;
};

