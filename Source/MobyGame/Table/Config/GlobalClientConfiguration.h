// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GlobalClientConfiguration.generated.h"

//���߻��ṩһЩ����������
USTRUCT(BlueprintType)
struct FGlobalClientConfiguration :public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FGlobalClientConfiguration();

	UPROPERTY(EditDefaultsOnly, Category = "MiniMap")
	float MiniMiniMapSize;

	UPROPERTY(EditDefaultsOnly, Category = "MiniMap")
	float MaxMiniMapSize;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MaxTargetArmLength;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MiniTargetArmLength;
};