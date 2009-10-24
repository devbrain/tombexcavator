class BulletClass
{
public:
	BulletClass(ObjectClass* newShooter, COORDTYPE* newLocation, COORDTYPE* newDestination, int newItemID, bool air);
	~BulletClass();
	void blitToScreen();

	void updatePosition();
	void destroy();
	void load(FILE* fp);
	void save(FILE* fp);


	inline bool isDirectShot() { return ((source.x == destination.x) && (source.y == destination.y)); }
	inline bool outOfRange() { return (frameTimer == 0); }
	inline int getDrawnX() { return ((int)(realX - xOffset) - dborder->minX); }
	inline int getDrawnY() { return ((int)(realY - yOffset) - dborder->minY); }
	inline int getItemID() { return itemID; }
	inline int getImageW() { return imageW; }
	inline int getImageH() { return imageH; }
	inline double getSpeed() { return speed; }
	inline double getRealX() { return realX; }
	inline double getRealY() { return realY; }

private:

	bool	airAttack,

			destroyed,

			groundBlocked;



	int	damage,

		damagePiercing, //from 0 for none to 10 max

		damageRadius,

		deathFrame,

		deathSound,
		drawnAngle,
		imageW, imageH,
		itemID,

		numDeathFrames,
		numFrames,
		xOffset,
		yOffset,

		frameTime,
		frameTimer;
	
	double	realX, realY,
			speed,
			angle,
			destAngle,
			angleLeft,
			angleRight,
			maxSpeed,
			acceleration,
			initialSpeed,
			xSpeed, initialXSpeed,
			ySpeed, initialYSpeed,
			turnSpeed,
			distanceTraveled;

	SDL_Surface*	graphic;

	COORDTYPE	destination,

				location,

				source;

	ObjectClass*	shooter;
	PlayerClass*	owner;
	SDL_Surface		*deathGraphic[5][5];
};

