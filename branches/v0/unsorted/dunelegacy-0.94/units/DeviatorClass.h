class DeviatorClass : public LauncherClass
{
public:
	DeviatorClass(PlayerClass* newOwner);
	~DeviatorClass();

	bool canAttack(ObjectClass* object);
};

