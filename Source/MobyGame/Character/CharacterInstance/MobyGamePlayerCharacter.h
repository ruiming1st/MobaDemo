// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MobyGameCharacter.h"
#include "MobyGamePlayerCharacter.generated.h"

class UStaticMeshComponent;
/**
 * 
 */
UCLASS()
class MOBYGAME_API AMobyGamePlayerCharacter : public AMobyGameCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TipComponent;
public:
	FSimpleDelegate DieDelegate;

	AMobyGamePlayerCharacter();
	virtual void InitCharacter();
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void SkillAttack(int32 InSkillID);

	UAnimMontage* GetCurrentSkillMontage(int32 InSkillID);

	void InitWidgetInfoOnClient();
	void AddExp(float InExp);
public:
	UFUNCTION(NetMulticast, unreliable)
	void MultiCastWidgetInfo_PlayerName(const FString &InPlayerName);

	virtual void Die();
	void SetSkillTip(UMaterialInterface* InMaterialInterface);

	UFUNCTION(NetMulticast, unreliable)
	void MultiCastWidgetInfo_Lv(const int32 &InCharacterLv);

public:
	bool bPressSkillKey;
};
