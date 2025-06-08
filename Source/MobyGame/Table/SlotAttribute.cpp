#include "SlotAttribute.h"

FSlotAttribute::FSlotAttribute() : FMobyGameTableBase()
{
	Type = ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED;
	Lv = 0;
	ConsumeMana = 0.f;
	ConsumeAnger = 0.f;
	Health.Value = 0.f;
	CD.Value = 4.0f;
	BuffID = INDEX_NONE;
	SkillID = INDEX_NONE;
	Skill = NULL;
	Buff = NULL;
	EffectType = ESkillEffectType::EFFECT_NONE;
}

void FSlotAttribute::UpdateLv()
{
	if (AddLevelValue)
	{
		float Coefficient = 0.1f;

		Lv++;
		Mana.Value += (Lv - 1) * AddLevelValue->Mana.Value * Coefficient;
		Anger.Value += (Lv - 1) * AddLevelValue->Anger.Value * Coefficient;
		Health.Value += (Lv - 1) * AddLevelValue->Health.Value * Coefficient;
		PhysicalAttack.Value += (Lv - 1) * AddLevelValue->PhysicalAttack.Value * Coefficient;
		CriticalStrike.Value += (Lv - 1) * AddLevelValue->CriticalStrike.Value * Coefficient;
		Armor.Value += (Lv - 1) * AddLevelValue->Armor.Value * Coefficient;
		MagicAttack.Value += (Lv - 1) * AddLevelValue->MagicAttack.Value * Coefficient;
		MagicDefense.Value += (Lv - 1) * AddLevelValue->MagicDefense.Value * Coefficient;
		PhysicalPenetration.Value += (Lv - 1) * AddLevelValue->PhysicalPenetration.Value * Coefficient;
		MagicPenetration.Value += (Lv - 1) * AddLevelValue->MagicPenetration.Value * Coefficient;
		WalkSpeed.Value += (Lv - 1) * AddLevelValue->WalkSpeed.Value * Coefficient;
		AttackSpeed.Value += (Lv - 1) * AddLevelValue->AttackSpeed.Value * Coefficient;
		//EmpircalValue.Value += (Lv - 1) * AddLevelValue->EmpircalValue.Value * Coefficient;
		//EmpircalReward.Value += (Lv - 1) * AddLevelValue->EmpircalReward.Value * Coefficient;
	}
}

FSlotAttributeValue::FSlotAttributeValue()
{
	GainType = ESlotAttributeGainType::SLOTATTRIBUTETYPE_ADD;
	ValueType = ESlotAttributeValueType::SLOTATTRIBUTEVALUE_VALUE;
	Value = 0.f;
}
