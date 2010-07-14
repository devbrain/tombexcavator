class UnitClass : public ObjectClass
{
public:
	UnitClass(PlayerClass* newOwner);
	virtual ~UnitClass();
	void blitToScreen();
	virtual void checkPos() = 0;
	virtual void deploy(COORDTYPE* newLocation);
	virtual void destroy();
	void deviate(PlayerClass* newOwner);
	virtual void drawSelectionBox();
	virtual void frameChange();
	int getDrawnX();
	int getDrawnY();
	virtual void handleActionCommand(int xPos, int yPos);
	bool isInAttackModeRange(ObjectClass* object);
	void netDeviate(PlayerClass* newOwner);
	virtual void netDoCommand(ObjectClass* newTarget, COORDTYPE* dest, bool forced);
	virtual void netDoConsistancy();
	void netSetAttackMode(ATTACKTYPE newAttackMode);
	void repair();
	void resetStartPoint();
	void setAngle(int newAngle);
	void setAttackMode(ATTACKTYPE newAttackMode);
	void setDeviationTime(int newTime);
	virtual void setTarget(ObjectClass* newTarget);
	void setGettingRepaired();
	void setGuardPoint(COORDTYPE* newGuardPoint);
	void setGuardPoint(int newX, int newY);
	virtual void setLocation(int xPos, int yPos);
	virtual void setPickedUp(UnitClass* newCarrier);
	virtual void update();
	virtual bool canPass(int xPos, int yPos);

	inline void clearNextSpotFound() { nextSpotFound = false; }
	inline void clearPath() { pathList.clearList(); }
	inline void setAttacking(bool status) { attacking = status; }
	inline void setLocation(COORDTYPE* newLocation) { setLocation(newLocation->x, newLocation->y); }
	inline void setSpeedCap(double newSpeedCap) { speedCap = newSpeedCap; }
	inline bool isAttacking() { return attacking; }
	inline bool isTracked() { return tracked; }
	inline bool isTurreted() { return turreted; }
	inline bool isMoving() { return moving; }
	inline bool wasDeviated() { return (owner != realOwner); }
	inline int getAngle() { return drawnAngle; }
	inline int getDeviationTime() { return deviationTimer; }
	inline double getMaxSpeed() { return speed; }
	inline ATTACKTYPE getAttackMode() { return attackMode; }
	inline COORDTYPE* getGuardPoint() { return &guardPoint; }
	inline PlayerClass* getRealOwner() { return realOwner; }

protected:
	virtual void attack();
	virtual void engageTarget();
	virtual void move();
	virtual void navigate();
	void nodePushSuccesors(PriorityQ* open, TerrainClass* parent_node);
	virtual void setSpeeds();
	virtual void targeting();
	virtual void turn();
	void turnLeft();
	void turnRight();
	bool AStarSearch();
		
	bool	attacking,
			goingToRepairYard,
			justStoppedMoving,
			moving,
			nextSpotFound,
			pickedUp,
			turning,
			turreted,
			tracked;

	int	baseID,	//main colour changing graphic id num
		destAngle,
		deviationTimer,
		findTargetTimer,
		nextSpotAngle,
		noCloserPointCount,
		numWeapons,
		primaryWeaponReloadTime,
		primaryWeaponTimer,
		secondaryWeaponReloadTime,
		secondaryWeaponTimer,
		targetAngle,
		
		numAttackSounds,
		bulletType[2],
		attackSound[MAX_UNITSOUNDS];
                
	double	speed,
			speedCap,
			targetDistance,
			turnSpeed,
			xSpeed, ySpeed;

	COORDTYPE	nextSpot,
				guardPoint;

	LinkedList  pathList;
	PlayerClass*	realOwner;
};

