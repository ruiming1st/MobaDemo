// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NonMobyGamePlayer.h"
#include "MobyGameArchitectureCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API AMobyGameArchitectureCharacter : public ANonMobyGamePlayer
{
	GENERATED_BODY()
	
public:
	AMobyGameArchitectureCharacter();

	void SetPossibleAttack(bool bNewPossibleAttack);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	FORCEINLINE bool IsPossibleAttack() const {return bPossibleAttack;}
protected:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();
private:
	bool bPossibleAttack;

};
