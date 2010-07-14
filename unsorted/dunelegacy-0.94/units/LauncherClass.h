class LauncherClass : public TrackedUnit
{
public:
	LauncherClass(PlayerClass* newOwner);
	~LauncherClass();

	void blitToScreen();
	virtual bool canAttack(ObjectClass* object);

private:
	SDL_Surface*	turretGraphic;
};

