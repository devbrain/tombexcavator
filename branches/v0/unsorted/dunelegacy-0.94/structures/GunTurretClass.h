class GunTurretClass : public StructureClass
{
public:
	GunTurretClass(PlayerClass* newOwner);
	~GunTurretClass();
	void doSpecificStuff();
	virtual void handleActionCommand(int xPos, int yPos);
	void setTurretAngle(int newAngle);
	void turn();
	void turnLeft();
	void turnRight();
	virtual void attack();

	inline int getTurretAngle() { return round(angle); }

protected:
	int	attackSound,
		bulletType,
		findTargetTimer,
		primaryWeaponReloadTime;
};

