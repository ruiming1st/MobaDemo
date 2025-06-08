#include "Buff.h"

bool FSlotAttribute_Interior::Contains(int32 InKey)
{
	for (auto& Tmp : Attributes)
	{
		if (Tmp.Key == InKey)
		{
			return true;
		}
	}

	return false;
}

void FSlotAttribute_Interior::Add(int32 Key, const FSlotAttribute& InAttribute)
{
	Attributes.Add(FSlotAttribute_Interior::FSlotAttributeElement());
	Attributes[Attributes.Num() - 1].Key = Key;
	Attributes[Attributes.Num() - 1].Value = InAttribute;
}

void FSlotAttribute_Interior::Remove(int32 Key)
{
	if (Contains(Key))
	{
		FSlotAttribute_Interior::FSlotAttributeElement Element;
		for (auto& Tmp : Attributes)
		{
			if (Tmp.Key == Key)
			{
				Element = Tmp;
				break;
			}
		}

		if (Element.IsValid())
		{
			Attributes.Remove(Element);
		}
	}
}

void FSlotAttribute_Interior::SetKeyToNewKey(int32 OldKey, int32 NewKey)
{
	for (auto& Tmp : Attributes)
	{
		if (Tmp.Key == OldKey)
		{
			Tmp.Key = NewKey;
			break;
		}
	}
}

void FSlotAttribute_Interior::SwapKeyAKeyB(int32 AKey, int32 BKey)
{
	FSlotAttribute_Interior::FSlotAttributeElement* ElementA = NULL;
	FSlotAttribute_Interior::FSlotAttributeElement* ElementB = NULL;
	for (auto& Tmp : Attributes)
	{
		if (Tmp.Key == AKey)
		{
			ElementA = &Tmp;
		}
		else if (Tmp.Key == BKey)
		{
			ElementB = &Tmp;
		}

		if (ElementA && ElementB && ElementA != ElementB)
		{
			int32 ElementTmp = ElementA->Key;
			ElementA->Key = ElementB->Key;
			ElementB->Key = ElementTmp;
			break;
		}
	}
}

bool FSlotAttribute_Interior::FSlotAttributeElement::IsValid()
{
	return Key != INDEX_NONE;
}
