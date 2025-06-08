#pragma once

#define MOBA_BUFF(InValue1,InValue2) \
if (WeakPtr.IsValid()) \
{ \
	float NewValue = 0.f; \
	for (auto& Tmp : WeakPtr.Pin()->Attributes) \
	{ \
		if (Tmp.Value.Type != ESlotAttributeType::SLOTATTRIBUTETYPE_SKILL){ \
			if (Tmp.Value.##InValue2##.GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_ADD)\
			{ \
				NewValue += Tmp.Value.##InValue2##.Value; \
			} \
			else if (Tmp.Value.##InValue2##.GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_SUBTRACT) \
			{ \
				NewValue -= Tmp.Value.##InValue2##.Value; \
			}}\
	} \
	return InValue1 + NewValue; \
} \
return InValue1; 

#define MOBA_BUFF_GET(InValue) \
float FCharacterAttribute::Get##InValue##() const \
{ \
	MOBA_BUFF(InValue,InValue); \
}

#define MOBA_BUFF_GET_DIFF(InValue1,InValue2) \
float FCharacterAttribute::Get##InValue1##() const \
{ \
	MOBA_BUFF(InValue1,InValue2); \
}
