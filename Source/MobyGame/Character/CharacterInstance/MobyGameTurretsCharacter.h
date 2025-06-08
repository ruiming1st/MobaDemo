// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MobyGameCharacter.h"
#include "../../Data/System/AssistSystem.h"
#include "MobyGameArchitectureCharacter.h"
#include "MobyGameTurretsCharacter.generated.h"

/**
 * 
 */

UCLASS()
class MOBYGAME_API AMobyGameTurretsCharacter : public AMobyGameArchitectureCharacter
{
	GENERATED_BODY()

public:
	AMobyGameTurretsCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
public:
	//Controllerֻ�ڷ���������
	//ͬ����Ҫ���Replicated
	//��ʡ����ʹ��FVector2D������FRotator
	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector2D TowersRotator;

public:
	const TArray<FAssistPlayer>& GetAssists();
	void AddAssist(const int64& InPlayerID);
	const FAssistPlayer* GetLastAssist();

private:
	FAssistSystem AssistSystem;
};
