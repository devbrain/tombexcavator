#define FLAGMOVETIMER 25
#define ANIMMOVETIMER 50

class StructureClass : public ObjectClass
{
public:
	StructureClass(PlayerClass* newOwner);
	virtual ~StructureClass();


	void assignToMap(COORDTYPE* pos);
	void blitToScreen();

	void destroy();
	void drawGraphic();
	void drawSelectRect();

	void getCentrePoint(COORDTYPE* centrePoint);

	void getClosestCentrePoint(COORDTYPE* centrePoint, COORDTYPE* objectLocation);

	void setDestination(int newX, int newY);

	void setJustPlaced();
	void setFogged(bool truth){fogged = truth;};

	int getDrawnX();
	
	int getViewRange(){return ObjectClass::getViewRange();};

	int getDrawnY();
	virtual void handleActionCommand(int xPos, int yPos);
	void update();
	virtual void doSpecificStuff() = 0;
	void repair();
	static void repairCallback(void *val);

	COORDTYPE getClosestPoint(COORDTYPE* objectLocation);

	inline bool isABuilder() { return builder; }

	inline short getStructureSizeX() { return structureSize.x; }
	inline short getStructureSizeY() { return structureSize.y; }
	inline int getPowerRequirement() { return powerRequirement; }
	inline COORDTYPE* getStructureSize() { return &structureSize; }

	inline int getOriginalHouse() { return origHouse; }
	inline void setOriginalHouse(int i) { origHouse = i; }
protected:
	bool	builder,
			fogged,
			justPlaced,
			repairing;

	int		powerRequirement,
			wantedAngle;
	int		origHouse;//for takeover, we still want to keep track of what the original owner was
                
	double	timeToTarget,
			avb,
			ab,
			distanceToTarget,
			destAngle,
			angleLeft,
			angleRight,
			turnSpeed,
			speed,
			primaryWeaponTimer,
			secondaryWeaponTimer;

	COORDTYPE	structureSize;
	
	SDL_Rect lastVisible; /*store picture drawn before fogged*/
	
	SDL_Surface	***deathGraphic,
				*placeGraphic;
	Button repairButton;
};

