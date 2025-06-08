#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MobyGameTableBase.generated.h"

USTRUCT(BlueprintType)
struct FMobyGameTableBase : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FMobyGameTableBase();

	UPROPERTY(EditDefaultsOnly, Category = "Moby Game Table")
	int32 ID;
};