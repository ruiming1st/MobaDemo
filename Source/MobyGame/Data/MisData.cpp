#include "MisData.h"

FSlotData::FSlotData()
{
	Reset();
}

void FSlotData::Reset()
{
	bCancelBuy = true;
	SlotID = INDEX_NONE;
	SlotICO = NULL;
	CD = 0.f;
	Number = INDEX_NONE;
}



FKillNetPackage::FKillNetPackage()
	: KillType(EKillType::KILL)
	,KillerIcon(NULL)
	,KilledIcon(NULL)
{

}

FPlayerLocation::FPlayerLocation()
	:CharacterID(INDEX_NONE)
	, Location(FVector::ZeroVector)
{

}

FSkillUpNetPackage::FSkillUpNetPackage()
{
	bEnableCurrentSlot = true;
	Lv = INDEX_NONE;
	bHidAllSlot = false;
	SlotID = INDEX_NONE;
}

FPlayerInfoNetPackage::FPlayerInfoNetPackage() : DeathTime(0.f)
{

}
