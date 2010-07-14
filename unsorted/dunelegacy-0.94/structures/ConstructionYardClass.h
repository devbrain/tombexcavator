class ConstructionYardClass : public BuilderClass
{
public:
        ConstructionYardClass(PlayerClass* newOwner);
        ~ConstructionYardClass();

	void checkSelectionList();
	void doSpecificStuff();
};

