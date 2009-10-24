class RocketTurretClass : public GunTurretClass
{
public:
	RocketTurretClass(PlayerClass* newOwner);
	~RocketTurretClass();

	void doSpecificStuff();
	bool canAttack(ObjectClass* object);

private:
};

