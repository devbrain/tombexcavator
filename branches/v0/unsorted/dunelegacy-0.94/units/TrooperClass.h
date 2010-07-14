class TrooperClass : public InfantryClass
{
public:
	TrooperClass(PlayerClass* newOwner);
	~TrooperClass();

	bool canAttack(ObjectClass* object);

protected:
};

