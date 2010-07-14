class TrackedUnit : public GroundUnit
{
public:
	TrackedUnit(PlayerClass* newOwner);
	~TrackedUnit();
	void checkPos();
	bool canPass(int xPos, int yPos);
};

