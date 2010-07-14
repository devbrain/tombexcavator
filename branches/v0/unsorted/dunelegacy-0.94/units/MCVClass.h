class MCVClass : public GroundUnit
{
public:
	MCVClass(PlayerClass* newOwner);
	~MCVClass();

	void deploy();
	void netDeploy();
};

