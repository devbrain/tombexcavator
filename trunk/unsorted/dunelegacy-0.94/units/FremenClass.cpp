#include "../sand.h"
#include "../Items.h"

FremenClass::FremenClass(PlayerClass* newOwner) : TrooperClass(newOwner)
{
	//TrooperClass::TrooperClass(newOwner);
	baseID = itemID = Unit_Fremen;
	speed = 0.25;
}
FremenClass::~FremenClass()
{

}

