class WindTrapClass : public StructureClass
{
public:
        WindTrapClass(PlayerClass* newOwner);
	~WindTrapClass();

	void doSpecificStuff();

private:
	int colourTimer;
};

