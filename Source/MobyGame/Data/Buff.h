// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Table/SlotAttribute.h"

struct FSlotAttribute_Interior : public TSharedFromThis<FSlotAttribute_Interior>
{
	struct FSlotAttributeElement
	{
		int32 Key;
		FSlotAttribute Value;

		bool IsValid();

		bool operator==(const FSlotAttributeElement& InElement)
		{
			return Key == InElement.Key;
		}
	};

	bool Contains(int32 InKey);
	void Add(int32 Key, const FSlotAttribute& InAttribute);
	void Remove(int32 Key);

	void SetKeyToNewKey(int32 OldKey, int32 NewKey);//ÒÆ¶¯
	void SwapKeyAKeyB(int32 AKey, int32 BKey);//½»»»

	FSlotAttribute* operator[](int32 InID)
	{
		for (auto& Tmp : Attributes)
		{
			if (Tmp.Key == InID)
			{
				return &Tmp.Value;
			}
		}

		return NULL;
	}

	TArray<FSlotAttributeElement> Attributes;
};