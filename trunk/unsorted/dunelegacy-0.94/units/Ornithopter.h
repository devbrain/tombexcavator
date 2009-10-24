class Ornithopter : public AirUnit
{
public:
	Ornithopter(PlayerClass* newOwner);
	~Ornithopter();

	void checkPos();
	bool canAttack(ObjectClass* object);

protected:
};

