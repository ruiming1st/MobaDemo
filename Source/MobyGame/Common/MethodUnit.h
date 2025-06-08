#pragma once

#include "CoreMinimal.h"
#include "../MobyGameState.h"
#include "../PlayerComponent/PlayerDataComponent.h"
#include "../MobyGamePlayerState.h"
#include "../Character/CharacterInstance/MobyGameCharacter.h"
#include "../MobyPawn.h"

class UUI_Tip;
namespace MethodUnit
{
	enum EServerCallType :uint8
	{
		INPROGRESS,
		PROGRESS_COMPLETE
	};

	const FSlotAttribute* GetSlotAttributeByPlayer(AMobyGamePlayerState *InPlayerState, const UAnimMontage *InMontage);
	AMobyPawn* GetPawn(UWorld *NewWorld, const int64& InPlayerID);//服务端获取
	AMobyPawn* GetPawn(UWorld *NewWorld);//客户端获取
	AMobyGameState* GetGameState(UWorld *NewWorld);
	AMobyGamePlayerState* GetPlayerState(UWorld* NewWorld, const int64& InPlayerID);//服务端获取
	AMobyGamePlayerState* GetPlayerState(UWorld* NewWorld);//客户端获取
	FCharacterTable* GetCharacterTableByPlayerID(UWorld* NewWorld, int32 PlayerID);
	const FCharacterTable* GetCharacterTableByCharacterID(UWorld* NewWorld, int32 CharacterID);

	int32 GetCharacterID(UWorld* NewWorld, int64 InPlayerID);

	AMobyGameCharacter* GetGameCharacterForce(UWorld* NewWorld, const int64& InPlayerID);

	bool IsPlayer(UWorld* NewWorld, int64 InPlayerID);

	template<class T>
	void ServerCallAllPlayerController(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement);

	template<class T>
	void ServerCallAllPlayerState(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement);

	void ServerCallAllPlayerData(UWorld* NewWorld, TFunction<EServerCallType(const UPlayerDataComponent*)> InImplement);

	template<class T>
	void ServerCallAllPlayer(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement);

	template<class T>
	void ServerCallAllCharacterAI(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement);

	template<class T>
	void ServerCallAllCharacter(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement);

	template<class T>
	T* FindTargetInRange(UWorld* NewWorld, float InRange, T* Owner, bool bFriendly);

	//Tip
	bool SlotAttributeValueToDescription(const FSlotAttributeValue* InValue, FText& OutText, const FText& ColorTag, const FText& DescriptiveTag);
	bool GetAttributeDescription(UWorld *NewWorld, int32 InID, FText& OutText);
	bool GetBaseAttributeDescription(const FSlotAttribute* InAttribute, FText& OutText);

	void RegisterSlotAttribute(AMobyGamePlayerState* InPlayerState, FSlotAttribute& InSlotAttribute);
	//UI
	void TipRegistration(UUI_Tip* Tip, const FSlotAttribute* InSlotAttribute);
}