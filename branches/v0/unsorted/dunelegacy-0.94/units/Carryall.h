class Carryall : public AirUnit
{
public:
	Carryall(PlayerClass* newOwner);
	~Carryall();

	void checkPos();
	void destroy();
	void deployUnit(UnitClass* unit);
	void dropUnit(UnitClass* unit);
	void giveCargo(UnitClass* newObject);
	void handleActionCommand(int xPos, int yPos);
	void loadCargo(FILE* fp);
	void saveCargo(FILE* fp);
	void setTarget(ObjectClass* newTarget);
	bool hasCargo();

	void findConstYard();

	inline void book() { booked = true; }
	inline void setOwned(bool b) { owned = b; }
	inline void setDropOfferer(bool status) { aDropOfferer = status; respondable = !aDropOfferer; booked = aDropOfferer ? true : booked; }
	inline bool isBooked() { return (booked || hasCargo()); }

protected:
	void engageTarget();
	void pickupTarget();
	void targeting();
	void turn();

	bool	aDropOfferer,
			booked,
			droppedOffCargo;
	int		numCargo;
	ListIterator	pickedUpUnits;

	int			curFlyPoint;
	COORDTYPE	flyPoints[8];
	COORDTYPE	constYardPoint;
	bool idle;
	bool firstRun;
	bool owned;
};

