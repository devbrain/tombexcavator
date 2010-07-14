class HeavyFactoryClass : public BuilderClass
{
public:
        HeavyFactoryClass(PlayerClass* newOwner);
	~HeavyFactoryClass();

	void buildRandom();
	void checkSelectionList();
	void doSpecificStuff();

private:

};

