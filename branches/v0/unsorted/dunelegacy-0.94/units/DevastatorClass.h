class DevastatorClass : public TrackedUnit
{
public:
	DevastatorClass(PlayerClass* newOwner);
	~DevastatorClass();

	void blitToScreen();
	void devastate();
	void startDevastate();
	void netDevastate();
	void update();

private:
	bool			devastated;
	int				devastateTimer;
	SDL_Surface*	turretGraphic,
					*explosion[3][3];
};

