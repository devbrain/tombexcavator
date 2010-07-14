class GroundUnit : public UnitClass
{
public:
	GroundUnit(PlayerClass* newOwner);
	virtual ~GroundUnit();
	void bookCarrier(UnitClass* newCarrier);
	virtual void checkPos();
	void requestCarryall();
	void setPickedUp(UnitClass* newCarrier);
	void handleDamage(int damage, ObjectClass* damager);

	inline bool isBadlyDamaged() { return badlyDamaged; }
	inline void setAwaitingPickup(bool status) { awaitingPickup = status; }
	inline bool isAwaitingPickup() { return awaitingPickup; }
	inline bool hasBookedCarrier() { return (bookedCarrier != NULL); }
	inline UnitClass* getCarrier() { return bookedCarrier; }

protected:
	bool	awaitingPickup;

	void navigate();

	UnitClass*	bookedCarrier;
};

