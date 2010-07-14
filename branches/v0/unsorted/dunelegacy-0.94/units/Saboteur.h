class Saboteur : public InfantryClass
{
public:
	Saboteur(PlayerClass* newOwner);
	~Saboteur();

	void checkPos();
	void deploy(COORDTYPE* newLocation);
	bool canAttack(ObjectClass* object);

protected:
};

