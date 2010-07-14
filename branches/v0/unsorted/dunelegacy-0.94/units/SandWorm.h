#define SANDWORM_LENGTH 40
#define SANDWORM_SEGMENTS 4	//how many shimmer things will be drawn per worm

class Sandworm : public GroundUnit
{
public:
	Sandworm(PlayerClass* newOwner);
	~Sandworm();

	void assignToMap(COORDTYPE* pos);
	void attack();
	void blitToScreen();
	void checkPos();
	void frameChange();
	void setLocation(int xPos, int yPos);
	void setSleepTimer(int newSleepTime);
	void update();
	bool canAttack(ObjectClass* object);
	bool canPass(int xPos, int yPos);
	ObjectClass* findTarget();

	inline int getSleepTimer() { return sleepTimer; }

protected:
	void engageTarget();
	void sleep();

private:
	int	kills,
		numAttackFrames,
		sleepTimer;
	COORDTYPE lastLocs[SANDWORM_LENGTH];

	SDL_Surface** shimmerSurface;
};

