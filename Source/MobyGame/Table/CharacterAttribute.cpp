#include "CharacterAttribute.h"
#include "../PlayerComponent/PlayerDataComponent.h"
#include "../MobyGameMacro.h"

FCharacterAttribute::FCharacterAttribute() : FMobyGameTableBase()
{
	Init();
}

MOBA_BUFF_GET(AttackSpeed);
MOBA_BUFF_GET(PhysicalAttack);
MOBA_BUFF_GET(MagicAttack);
MOBA_BUFF_GET(WalkSpeed);
MOBA_BUFF_GET(MagicPenetration);
MOBA_BUFF_GET(PhysicalPenetration);
MOBA_BUFF_GET(MagicDefense);
MOBA_BUFF_GET_DIFF(MaxMana, Mana);
MOBA_BUFF_GET_DIFF(MaxAnger, Anger);
MOBA_BUFF_GET_DIFF(MaxHealth, Health);
MOBA_BUFF_GET(CriticalStrike);
MOBA_BUFF_GET(Armor);

void FCharacterAttribute::Init()
{
	Lv = 1;
	MaxMana = 100.0f;
	Mana = GetMaxMana();
	MaxAnger = 0.0f;
	Anger = 0.0f;
	MaxHealth = 555.f;
	Health = GetMaxHealth();
	PhysicalAttack = 55.f;
	CriticalStrike = 0.f;
	Armor = 30.f;
	MagicAttack = 0.0f;
	MagicDefense = 0.0f;
	PhysicalPenetration = 0.0f;
	MagicPenetration = 0.0f;
	MaxEmpircalValue = 100.f;
	EmpircalValue = 0.f;
	AttackSpeed = 0.66f;
	WalkSpeed = 356.f;
	RangeAttack = 1000.f;

	Coefficient = .1f;
	EmpircalReward = 20.f;
	GoldReward = 10.f;
	AddLevelID = INDEX_NONE;
	AddLevelValue = NULL;
}

void FCharacterAttribute::Reset()
{
	Health = GetMaxHealth();
	Mana = GetMaxMana();
}

float FCharacterAttribute::GetHealthPercentage() const
{
	return Health / GetMaxHealth();
}

float FCharacterAttribute::GetManaPercentage() const
{
	return Mana / GetMaxMana();
}

float FCharacterAttribute::GetEmpircalPercentage() const
{
	return EmpircalValue / MaxEmpircalValue;
}


float FCharacterAttribute::GetAngerPercentage() const
{
	return Anger / GetMaxAnger();
}

void FCharacterAttribute::SetBuff(TSharedRef<FSlotAttribute_Interior> InBuff)
{
	//只是用WeakPtr来弱引用MobyPlayerState中PlayerDataComponent中的FSlotAttribute_Interior实例
	WeakPtr = InBuff;
}

void FCharacterAttribute::UpdateLevel()
{
	if (AddLevelValue)
	{
		Lv++;
		MaxMana += (Lv - 1) * AddLevelValue->Mana.Value * Coefficient;
		MaxAnger += (Lv - 1) * AddLevelValue->Anger.Value * Coefficient;
		MaxHealth += (Lv - 1) * AddLevelValue->Health.Value * Coefficient;
		PhysicalAttack += (Lv - 1) * AddLevelValue->PhysicalAttack.Value * Coefficient;
		CriticalStrike += (Lv - 1) * AddLevelValue->CriticalStrike.Value * Coefficient;
		Armor += (Lv - 1) * AddLevelValue->Armor.Value * Coefficient;
		MagicAttack += (Lv - 1) * AddLevelValue->MagicAttack.Value * Coefficient;
		MagicDefense += (Lv - 1) * AddLevelValue->MagicDefense.Value * Coefficient;
		PhysicalPenetration += (Lv - 1) * AddLevelValue->PhysicalPenetration.Value * Coefficient;
		MagicPenetration += (Lv - 1) * AddLevelValue->MagicPenetration.Value * Coefficient;
		WalkSpeed += (Lv - 1) * AddLevelValue->WalkSpeed.Value * Coefficient;
		AttackSpeed += (Lv - 1) * AddLevelValue->AttackSpeed.Value * Coefficient;
		MaxEmpircalValue += (Lv - 1) * AddLevelValue->EmpircalValue.Value * Coefficient;
		EmpircalReward += (Lv - 1) * AddLevelValue->EmpircalReward.Value * Coefficient;
	}
}

bool FCharacterAttribute::IsLvUpperLimit() const
{
	return Lv >= 18;
}

float FCharacterAttribute::GetEmpircalReward() const
{
	return ((float)Lv) * (EmpircalReward + 0.5f);
}

int32 FCharacterAttribute::GetGoldReward() const
{
	//击杀系统 联系到一起

	//连续计算

	//对方是不是死成鬼

	return GoldReward;
}


bool FCharacterAttribute::UseMana(float InMana)
{
	if (Mana >= InMana)
	{
		Mana -= InMana;
		return true;
	}
	return false;
}

bool FCharacterAttribute::UseAnger(float InAnger)
{
	if (Anger >= InAnger)
	{
		Anger -= InAnger;
		return true;
	}
	return false;
}
