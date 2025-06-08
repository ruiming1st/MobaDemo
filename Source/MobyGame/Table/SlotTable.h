#pragma once

#include "CoreMinimal.h"
#include "Core/MobyGameTableBase.h"
#include "SlotTable.generated.h"

UENUM(BlueprintType)
enum class ESlotType :uint8
{
	SLOT_ALL					UMETA(DisplayName = "All"),
	SLOT_CONSUMABLES			UMETA(DisplayName = "Consumables"),
	SLOT_AUXILIARYPROPS			UMETA(DisplayName = "AuxiliaryProps"),
	SLOT_PHYSICALATTACK			UMETA(DisplayName = "PhysicalAttack"),
	SLOT_MAGICATTACK			UMETA(DisplayName = "MagicAttack"),
	SLOT_PHYSICALPENETRATION	UMETA(DisplayName = "PhysicalPenetration"),
	SLOT_MAGICPENETRATION		UMETA(DisplayName = "MagicPenetration"),
	SLOT_CRIT					UMETA(DisplayName = "Crit"),
	SLOT_ARMOR					UMETA(DisplayName = "Armor"),
	SLOT_MAGICRESISTANCE		UMETA(DisplayName = "MagicResistance"),
	SLOT_ATTACKSPEED			UMETA(DisplayName = "AttackSpeed"),
	SLOT_SHOES					UMETA(DisplayName = "Shoes"),
};

class ABullet;
USTRUCT(BlueprintType)
struct FSlotTable : public FMobyGameTableBase
{
	GENERATED_USTRUCT_BODY()

	FSlotTable();

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	FName SlotName;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	UTexture2D* SlotIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	TSubclassOf<ABullet> SlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	int32 SlotGold;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	FText SlotIntroduction;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	TArray<ESlotType> SlotType;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	TArray<int32> Childrens;
};