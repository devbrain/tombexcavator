#ifndef DUNE_OBJECTCLASS_H
#define DUNE_OBJECTCLASS_H

#define VIS_ALL -1



extern int nextObjectID;



class ObjectClass
{
public:
	ObjectClass(PlayerClass* newOwner);
	virtual ~ObjectClass();

	virtual void assignToMap(COORDTYPE* pos);
	virtual void blitToScreen() = 0;

	virtual void destroy() = 0;

	virtual void drawInterface();

	void drawSmoke(int x, int y);

	virtual void getClosestCentrePoint(COORDTYPE* centrePoint, COORDTYPE* objectLocation);

	virtual bool canAttack(ObjectClass* object);

	virtual int getDrawnX() = 0;

	virtual int getDrawnY() = 0;

	void getCentrePoint(COORDTYPE* centrePoint);
	virtual void handleDamage(int damage, ObjectClass* damager);
	virtual void handleActionCommand(int xPos, int yPos) = 0;

	virtual void handleInterfaceEvent(SDL_Event* event);

	void netDestroy();

	virtual void netDoAttack(ObjectClass* newTarget);

	virtual void netDoCommand(ObjectClass* newTarget, COORDTYPE* dest, bool forced);

	void playSelectSound();

	void playConfirmSound();

	void removeFromSelectionLists();

	virtual void repair() = 0;

	virtual void setDestination(int newX, int newY);

	void setHealth(int newHealth);

	virtual void setLocation(int xPos, int yPos);

	void setObjectID(int newObjectID);

	virtual void setTarget(ObjectClass* newTarget);

	void setVisible(int team, bool status);

	virtual void update() = 0;

	void unassignFromMap(COORDTYPE* location);

	bool isOnScreen();

	bool isVisible(int team);

	int getHealthColour();

	virtual COORDTYPE getClosestPoint(COORDTYPE* objectLocation);

	static ObjectClass* findClosestTargetStructure(ObjectClass* object);

	static ObjectClass* findClosestTargetUnit(ObjectClass* object);

	virtual ObjectClass* findTarget();

	inline void addHealth() { if (health < maxHealth) health+= 1.0; }
	inline void setActive(bool status) { active = status; }
	inline void setForced(bool status) { forced = status; }
	inline void setRespondable(bool status) { respondable = status; }
	inline void setSelected(bool value) { selected = value; }

	inline void setDestination(COORDTYPE* location) { if (location != NULL) setDestination(location->x, location->y); }

	inline void setLocation(COORDTYPE* location) { setLocation(location->x, location->y); }
	inline bool canAttack() { return canAttackStuff; }
	inline bool hasATarget() { return (target != NULL); }

	inline bool hasInterface() { return (interfaceContainer != NULL); }

	inline bool hasObjectID(int id) { return (objectID == id); }

	inline bool isActive() { return active; }

	inline bool isAFlyingUnit() { return aFlyingUnit; }

	inline bool isAGroundUnit() { return aGroundUnit; }
	inline bool isAStructure() { return aStructure; }
	inline bool isABuilder() { return aBuilder; }
	inline bool isInfantry() { return infantry; }

	inline bool isAUnit() { return aUnit; }
	inline bool isRespondable() { return respondable; }
	inline bool isSelected() { return selected; }
	inline bool outOfRange() { return (destroyed && (frameTimer == 0)); }
	inline bool wasDestroyed() { return destroyed; }
	inline bool wasForced() { return forced; }
	inline int getItemID() { return itemID; }
	inline int getX() { return location.x; }
	inline int getY() { return location.y; }
	inline int getArmour() { return armour; }
	inline int getGuardRange() { return guardRange; }
	inline int getHealth() { return round(health); }
	inline int getImageW() { return imageW; }
	inline int getImageH() { return imageH; }
	inline int getMaxHealth() { return maxHealth; }
	inline int getObjectID() { return objectID; }
	inline int getRadius() { return radius; }
	/*inline*/ int getViewRange();
	inline double getRealX() { return realX; }
	inline double getRealY() { return realY; }
	inline COORDTYPE* getLocation() { return &location; }
	inline COORDTYPE* getDestination() { return &destination; }
	inline Container* getInterface() { return interfaceContainer; }
	inline ObjectClass* getTarget() { return target; }
	inline PlayerClass* getOwner() { return owner; }
	inline void setOwner(PlayerClass* no) { owner = no; }
	
	void clearObject();// { return (destroyed && (frameTimer == 0)); }

protected:

	inline bool targetInWeaponRange() { return (blockDistance(&location, &target->getClosestPoint(&location)) <= weaponRange); }


	bool	active,

			aFlyingUnit,

			aGroundUnit,
			aStructure,

			aBuilder,

			aUnit,
			canAttackStuff,	//can i do damage to stuff?
			badlyDamaged,		//if below 50% health
			destroyed,
			forced,
			infantry,
			respondable,
			selected,
			targetFriendly,

			visible[MAX_PLAYERS+1];

	int	armour,

		checkTimer,

		counter,

		animcounter,

		animFrames,

		animFrame,

		isAnimating,

		deathFrame,

		drawnAngle,

		drawnFrame,

		frameTime,

		frameTimer,

		guardRange,

		imageW,
		imageH,

		itemID,
		maxHealth,

		numDeathFrames,

		objectID,
		radius,
		smokeCounter,
		smokeFrame,
		smokeLast,
		viewRange,

		weaponRange,

		xOffset,

		yOffset,

		

		numConfirmSounds,

		numDestroySounds,

		numSelectSounds,

		confirmSound[MAX_UNITSOUNDS],

		destroySound[MAX_UNITSOUNDS],

		selectSound[MAX_UNITSOUNDS];

	double	angle,
			health,
			realX,
			realY;

	ATTACKTYPE	attackMode;
	COORDTYPE	destination, location, oldLocation;
	Container	*interfaceContainer;
	ObjectClass	*target;
	PlayerClass	*owner;
	SDL_Surface	*graphic;
};

#endif //#define DUNE_OBJECTCLASS_H

