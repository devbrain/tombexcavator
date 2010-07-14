class WallClass : public StructureClass
{
public:
        WallClass(PlayerClass* newOwner);
	~WallClass();

	void checkSelf();
	void doSpecificStuff();

	inline void setTile(int newTile) { animFrame = newTile; }
};

