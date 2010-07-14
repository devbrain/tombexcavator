class SonicTankClass : public TrackedUnit
{
public:
	SonicTankClass(PlayerClass* newOwner);
	~SonicTankClass();

	void blitToScreen();
	void handleDamage(int damage, ObjectClass *damager);

	bool canAttack(ObjectClass* object);

private:
	SDL_Surface*	turretGraphic;
};

