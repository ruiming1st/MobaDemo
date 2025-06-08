#include "NumericalCalculationUnit.h"
#include "../Character/CharacterInstance/MobyGameCharacter.h"

namespace NumericalCalculationUnit
{
	float GetArmor(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute)
	{
		if (FCharacterAttribute* InEnemyData = InEnemy->GetCharacterAttribute())
		{
			if (FCharacterAttribute* OriginalData = InTarget->GetCharacterAttribute())
			{
				float PhysicalPenetration = InEnemyData->GetPhysicalPenetration();
				if (InSlotAttribute)
				{
					PhysicalPenetration = GetSlotAttributeValue(InSlotAttribute->PhysicalPenetration, PhysicalPenetration);
				}

				return OriginalData->GetArmor() * (1.f - PhysicalPenetration / 100.f);
			}
		}

		return 0;
	}

	float GetMagicDefense(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute)
	{
		if (FCharacterAttribute* InEnemyData = InEnemy->GetCharacterAttribute())
		{
			if (FCharacterAttribute* OriginalData = InTarget->GetCharacterAttribute())
			{
				float MagicPenetration = InEnemyData->GetMagicPenetration();
				if (InSlotAttribute)
				{
					MagicPenetration = GetSlotAttributeValue(InSlotAttribute->MagicPenetration, MagicPenetration);
				}

				return OriginalData->GetMagicDefense() *(1.f - MagicPenetration / 100.f);
			}
		}

		return 0;
	}

	float GetAttack(AMobyGameCharacter* InOriginal, const FSlotAttribute* InSlotAttribute)
	{
		if (FCharacterAttribute* OriginalData = InOriginal->GetCharacterAttribute())
		{
			float PhysicalAttack = OriginalData->GetPhysicalAttack();
			float CriticalStrike = OriginalData->GetCriticalStrike();
			if (InSlotAttribute)
			{
				PhysicalAttack = GetSlotAttributeValue(InSlotAttribute->PhysicalAttack, PhysicalAttack);
				CriticalStrike = GetSlotAttributeValue(InSlotAttribute->CriticalStrike, CriticalStrike);
			}

			return PhysicalAttack * (1 + CriticalStrike) + OriginalData->Lv;
		}

		return 0;
	}

	//对方给我造成的伤害
	float GetPhysicalDamage(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute)
	{
		if (FCharacterAttribute* InEnemyData = InEnemy->GetCharacterAttribute())
		{
			return GetAttack(InEnemy, InSlotAttribute) * (100 / (100 + GetArmor(InTarget, InEnemy, InSlotAttribute)));
		}

		return 0;
	}

	//对方给我造成的伤害
	float GetMagicDamage(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute)
	{
		if (FCharacterAttribute* InEnemyData = InEnemy->GetCharacterAttribute())
		{
			if (FCharacterAttribute* OriginalData = InTarget->GetCharacterAttribute())
			{
				float MagicAttackBaseValue = InEnemyData->GetMagicAttack();
				if (InSlotAttribute)
				{
					MagicAttackBaseValue = GetSlotAttributeValue(InSlotAttribute->MagicAttack, MagicAttackBaseValue);
				}

				return MagicAttackBaseValue * (100 / (100 + GetMagicDefense(InTarget, InEnemy, InSlotAttribute)));
			}
		}

		return 0;
	}

	//对方给我造成的伤害
	float GetDamage(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute)
	{
		return GetPhysicalDamage(InTarget, InEnemy, InSlotAttribute) + GetMagicDamage(InTarget, InEnemy, InSlotAttribute);
	}

	//通过传进来的值判断值是什么类型
	float GetSlotAttributeValue(const FSlotAttributeValue& InValue, float InNewValue)
	{
		if (InValue.ValueType == ESlotAttributeValueType::SLOTATTRIBUTEVALUE_VALUE)
		{
			if (InValue.GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_ADD)
			{
				InNewValue += InValue.Value;
			}
			else if (InValue.GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_SUBTRACT)
			{
				InNewValue += (-1.f * InValue.Value);
			}
		}
		else if (InValue.ValueType == ESlotAttributeValueType::SLOTATTRIBUTEVALUE_PERCENTAGE)
		{
			if (InValue.GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_ADD)
			{
				if (InValue.Value > 1.f)
				{
					InNewValue *= 2.f;
				}
				else if (InValue.Value > 0.f && InValue.Value <= 1.f)
				{
					InNewValue += InValue.Value * InNewValue;
				}
			}
			else if (InValue.GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_SUBTRACT)
			{
				InNewValue += -1.f * InValue.Value * InNewValue;
			}
		}

		if (InNewValue < 0.f)
		{
			InNewValue = 0.f;
		}

		return InNewValue;
	}
}