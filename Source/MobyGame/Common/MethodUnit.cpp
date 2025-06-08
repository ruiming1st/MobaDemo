#include "MethodUnit.h"
#include "EngineUtils.h"
#include "../UI/Game/Tip/UI_Tip.h"

#define LOCTEXT_NAMESPACE "MethodUnit"
namespace MethodUnit
{
	const FSlotAttribute* GetSlotAttributeByPlayer(AMobyGamePlayerState *InPlayerState, const UAnimMontage *InMontage)
	{
		if (InPlayerState)
		{
			//I.先找SkillID
			int32 SkillID = INDEX_NONE;
			if (AMobyGameState * InState = GetGameState(InPlayerState->GetWorld()))
			{
				SkillID = InState->GetCharacterSkillAttributeID(InMontage);
			}

			//II.通过ID寻找属性
			if (SkillID != INDEX_NONE)
			{
				return InPlayerState->GetSlotAttributeBySkillID(SkillID);
			}
		}

		return NULL;
	}

	AMobyPawn* GetPawn(UWorld *NewWorld, const int64& InPlayerID)
	{
		AMobyPawn* InNewPawn = NULL;
		ServerCallAllPlayer<AMobyPawn>(NewWorld, [&](AMobyPawn* InPawn)->EServerCallType
		{
			if (InPawn->GetPlayerID() == InPlayerID)
			{
				InNewPawn = InPawn;
				return EServerCallType::PROGRESS_COMPLETE;
			}

			return EServerCallType::INPROGRESS;
		});

		return InNewPawn;
	}

	AMobyPawn* GetPawn(UWorld *NewWorld)
	{
		if (NewWorld && NewWorld->GetFirstPlayerController())
		{
			return NewWorld->GetFirstPlayerController()->GetPawn<AMobyPawn>();
		}

		return NULL;
	}

	AMobyGameState* GetGameState(UWorld* NewWorld)
	{
		if (NewWorld)
		{
			return NewWorld->GetGameState<AMobyGameState>();
		}

		return NULL;
	}

	AMobyGamePlayerState* GetPlayerState(UWorld* NewWorld, const int64& InPlayerID)
	{
		AMobyGamePlayerState* InNewPlayerState = NULL;
		ServerCallAllPlayerState<AMobyGamePlayerState>(NewWorld, [&](AMobyGamePlayerState* InPlayerState)->EServerCallType
		{
			if (InPlayerState->GetPlayerData()->PlayerID == InPlayerID)
			{
				InNewPlayerState = InPlayerState;
				return EServerCallType::PROGRESS_COMPLETE;
			}

			return EServerCallType::INPROGRESS;
		});

		return InNewPlayerState;
	}

	AMobyGamePlayerState* GetPlayerState(UWorld* NewWorld)
	{
		if (NewWorld && NewWorld->GetFirstPlayerController())
		{
			return NewWorld->GetFirstPlayerController()->GetPlayerState<AMobyGamePlayerState>();
		}

		return NULL;
	}

	const FCharacterTable* GetCharacterTableByPlayerID(UWorld* NewWorld, int64 PlayerID)
	{
		int32 CharacterID = GetCharacterID(NewWorld, PlayerID);

		if (CharacterID != INDEX_NONE)
		{
			return GetCharacterTableByCharacterID(NewWorld, CharacterID);
		}

		return NULL;
	}

	const FCharacterTable* GetCharacterTableByCharacterID(UWorld* NewWorld, int32 InCharacterID)
	{
		if (AMobyGameState* InGameState = GetGameState(NewWorld))
		{
			return InGameState->GetCharacterTableTemplate(InCharacterID);
		}

		return NULL;
	}
	
	//角色数据保存在Gamestate中
	FCharacterAttribute* GetCharacterAttributeByPlayerID(UWorld* NewWorld, int64 InPlayerID)
	{
		if (AMobyGameState* InGameState = GetGameState(NewWorld))
		{
			return InGameState->GetCharacterAttribute(InPlayerID);
		}

		return NULL;
	}

	int32 GetCharacterID(UWorld* NewWorld, int64 InPlayerID)
	{
		if (AMobyGameState* InState = GetGameState(NewWorld))
		{
			return InState->GetCharacterID(InPlayerID);
		}

		return INDEX_NONE;
	}

	AMobyGameCharacter* GetGameCharacterForce(UWorld* NewWorld, const int64& InPlayerID)
	{
		for (TActorIterator<AMobyGameCharacter> Iter(NewWorld, AMobyGameCharacter::StaticClass()); Iter; ++Iter)
		{
			if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(*Iter))
			{
				if (InCharacter->GetPlayerID() == InPlayerID)
				{
					return InCharacter;
				}
			}
		}

		return NULL;
	}

	bool IsPlayer(UWorld* NewWorld, int64 InPlayerID)
	{
		bool bPlayer = false;
		ServerCallAllPlayerData(NewWorld, [&](const UPlayerDataComponent* InPlayerData)->EServerCallType
		{
			if (InPlayerData->PlayerID == InPlayerID)
			{
				bPlayer = true;
				return EServerCallType::PROGRESS_COMPLETE;
			}

			return EServerCallType::INPROGRESS;
		});

		return bPlayer;
	}

	//获得controller给调用此函数处传入的函数作为参数使用。
	template<class T>
	void ServerCallAllPlayerController(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		for (FConstPlayerControllerIterator It = NewWorld->GetPlayerControllerIterator(); It; ++It)
		{
			if (T* InPlayerController = Cast<T>(It->Get()))
			{
				if (InImplement(InPlayerController) == EServerCallType::PROGRESS_COMPLETE)
				{
					break;
				}
			}
		}
	}

	template<class T>
	void ServerCallAllPlayerState(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		ServerCallAllPlayerController<APlayerController>(NewWorld, [&](APlayerController* InPlayerContoroller)->EServerCallType
		{
			if (T* InState = InPlayerContoroller->GetPlayerState<T>())
			{
				return InImplement(InState);
			}

			return EServerCallType::PROGRESS_COMPLETE;
		});
	}

	void ServerCallAllPlayerData(UWorld* NewWorld, TFunction<EServerCallType(const UPlayerDataComponent*)> InImplement)
	{
		ServerCallAllPlayerState<AMobyGamePlayerState>(NewWorld, [&](AMobyGamePlayerState* InPlayerState)->EServerCallType
		{
			if (InPlayerState->GetPlayerData())
			{
				return InImplement(InPlayerState->GetPlayerData());
			}

			return EServerCallType::PROGRESS_COMPLETE;
		});
	}

	template<class T>
	void ServerCallAllPlayer(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		ServerCallAllPlayerController<APlayerController>(NewWorld, [&](APlayerController* InPlayerContoroller)->EServerCallType
		{
			if (T* InPawn = Cast<T>(InPlayerContoroller->GetPawn()))
			{
				return InImplement(InPawn);
			}

			return EServerCallType::PROGRESS_COMPLETE;
		});
	}

	template<class T>
	void ServerCallAllCharacterAI(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		ServerCallAllPlayer<AMobyPawn>(NewWorld, [&](AMobyPawn* InPawn)->EServerCallType
		{
			return InImplement(InPawn->GetControllerCharacter());
		});
	}

	template<class T>
	void ServerCallAllCharacter(UWorld* NewWorld, TFunction<EServerCallType(T*)> InImplement)
	{
		for (TActorIterator<T> It(NewWorld, T::StaticClass()); It; ++It)
		{
			if (T* InCharacter = Cast<T>(*It))
			{
				if (InImplement(InCharacter) == EServerCallType::PROGRESS_COMPLETE)
				{
					break;
				}
			}
		}
	}

	template<class T>
	T* FindTargetInRange(UWorld* NewWorld, float InRange, T* Owner, bool bFriendly)
	{
		T* Instance = NULL;
		if (NewWorld && Owner && InRange != 0.f)
		{
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(NewWorld, AMobyGameCharacter::StaticClass(), OutActors);

			auto CheckTeamType = [&](T* InCharacter)
			{
				if (bFriendly)
				{
					return InCharacter->GetTeam() == Owner->GetTeam();
				}

				return  InCharacter->GetTeam() != Owner->GetTeam();
			};


			float DistanceRef = 999999.f;
			for (auto &Tmp : OutActors)
			{
				if (Tmp != Owner)
				{
					if (T* InCharacter = Cast<T>(Tmp))
					{
						if (!InCharacter->IsDie())
						{
							if (CheckTeamType(InCharacter))
							{
								float Distance = FVector::Dist(InCharacter->GetActorLocation(), Owner->GetActorLocation());
								if (Distance < DistanceRef)
								{
									DistanceRef = Distance;
									if (DistanceRef <= InRange)
									{
										Instance = InCharacter;
									}
								}

							}
						}
					}
				}
			}
		}

		return Instance;
	}

	bool SlotAttributeValueToDescription(const FSlotAttributeValue* InValue, FText& OutText, const FText& ColorTag, const FText& DescriptiveTag)
	{
		if (InValue->Value != 0.f)
		{
			if (InValue->GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_ADD)
			{
				if (InValue->ValueType == ESlotAttributeValueType::SLOTATTRIBUTEVALUE_PERCENTAGE)
				{
					OutText = FText::Format(LOCTEXT("SlotAttribute_add_percentage", "add <{0}> +{1}%</> overall {2}"), ColorTag, FText::AsNumber((int32)InValue->Value * 100.f), DescriptiveTag);
				}
				else if (InValue->ValueType == ESlotAttributeValueType::SLOTATTRIBUTEVALUE_VALUE)
				{
					OutText = FText::Format(LOCTEXT("SlotAttribute_add_value", "increase <{0}> +{1}</> {2}"), ColorTag, FText::AsNumber((int32)InValue->Value), DescriptiveTag);
				}
			}
			else if (InValue->GainType == ESlotAttributeGainType::SLOTATTRIBUTETYPE_SUBTRACT)
			{
				if (InValue->ValueType == ESlotAttributeValueType::SLOTATTRIBUTEVALUE_PERCENTAGE)
				{
					OutText = FText::Format(LOCTEXT("SlotAttribute_subtract_percentage", " cause <Red> -{0}%</> {1} to the other party "), FText::AsNumber((int32)(InValue->Value * 100.f)), DescriptiveTag);
				}
				else if (InValue->ValueType == ESlotAttributeValueType::SLOTATTRIBUTEVALUE_VALUE)
				{
					OutText = FText::Format(LOCTEXT("SlotAttribute_subtract_value", " cause <Red> -{0}</> {1} to the opponent "), FText::AsNumber((int32)InValue->Value), DescriptiveTag);
				}
			}

		}

		return !OutText.IsEmpty();
	}

	//BuffID? ID?
	bool GetAttributeDescription(UWorld *NewWorld, int32 InID, FText& OutText)
	{
		if (InID != INDEX_NONE)
		{
			if (AMobyGamePlayerState* InPlayerState = GetPlayerState(NewWorld))
			{
				if (const FSlotAttribute* InAttribute = InPlayerState->GetSlotAttributeTemplate(InID))
				{
					return GetBaseAttributeDescription(InAttribute, OutText);
				}
			}
		}

		return false;
	}

	bool GetBaseAttributeDescription(const FSlotAttribute* InAttribute, FText& OutText)
	{
		if (InAttribute)
		{
			//Health
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->Health, Attribute, FText::FromString(TEXT("Green")), LOCTEXT("Attr_HP", "Health")))
				{
					OutText = FText::Format(LOCTEXT("AttributeValue_Health", "{0} {1}"), OutText, Attribute);
				}
			}

			//Mana
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->Mana, Attribute, FText::FromString(TEXT("Blue")), LOCTEXT("Attr_MP", "Mana")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_Mana", "{0}{1}"), OutText, Attribute);
				}
			}
			//PhysicalAttack
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->PhysicalAttack, Attribute, FText::FromString(TEXT("Brown")), LOCTEXT("Attr_PhysicalAttack", "PhysicalAttack")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_PhysicalAttack", "{0}{1}"), OutText, Attribute);
				}
			}
			//CriticalStrike
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->CriticalStrike, Attribute, FText::FromString(TEXT("Gules")), LOCTEXT("Attr_CriticalStrike", "CriticalStrike")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_CriticalStrike", "{0}{1}"), OutText, Attribute);
				}
			}
			//Anger
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->Anger, Attribute, FText::FromString(TEXT("Yellow")), LOCTEXT("Attr_Anger", "Anger")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_Anger", "{0}{1}"), OutText, Attribute);
				}
			}
			//Armor
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->Armor, Attribute, FText::FromString(TEXT("DarkBrown")), LOCTEXT("Attr_Armor", "Armor")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_Armor", "{0}{1}"), OutText, Attribute);
				}
			}
			//MagicAttack
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->MagicAttack, Attribute, FText::FromString(TEXT("LightBlue")), LOCTEXT("Attr_MagicAttack", "MagicAttack")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_MagicAttack", "{0}{1}"), OutText, Attribute);
				}
			}
			//MagicDefense
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->MagicDefense, Attribute, FText::FromString(TEXT("Navyblue")), LOCTEXT("Attr_MagicDefense", "MagicDefense")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_MagicDefense", "{0}{1}"), OutText, Attribute);
				}
			}
			//PhysicalPenetration
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->PhysicalPenetration, Attribute, FText::FromString(TEXT("LightBrown")), LOCTEXT("Attr_PhysicalPenetration", "PhysicalPenetration")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_PhysicalPenetration", "{0}{1}"), OutText, Attribute);
				}
			}
			//MagicPenetration
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->MagicPenetration, Attribute, FText::FromString(TEXT("PurpleBlue")), LOCTEXT("Attr_MagicPenetration", "MagicPenetration")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_MagicPenetration", "{0}{1}"), OutText, Attribute);
				}
			}
			//WalkSpeed
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->WalkSpeed, Attribute, FText::FromString(TEXT("LightGreen")), LOCTEXT("Attr_WalkSpeed", "WalkSpeed")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_WalkSpeed", "{0}{1}"), OutText, Attribute);
				}
			}
			//AttackSpeed
			{
				FText Attribute;
				if (SlotAttributeValueToDescription(&InAttribute->AttackSpeed, Attribute, FText::FromString(TEXT("BrownYellow")), LOCTEXT("Attr_AttackSpeed", "AttackSpeed")))
				{
					OutText = FText::Format(LOCTEXT("BaseAttribute_AttackSpeed", "{0}{1}"), OutText, Attribute);
				}
			}

			return true;
		}

		return false;
	}

	//客户端缓存服务器数据
	void RegisterSlotAttribute(AMobyGamePlayerState* InPlayerState, FSlotAttribute& InSlotAttribute)
	{
		//等级添加注册
		if (InSlotAttribute.AddLevelID != INDEX_NONE)
		{
			if (const FSlotAttribute *InLvSlotAttribute = InPlayerState->GetSlotAttributeTemplate(InSlotAttribute.AddLevelID))
			{
				InSlotAttribute.AddLevelValue = InLvSlotAttribute;
			}
		}

		if (InSlotAttribute.BuffID != INDEX_NONE)
		{
			if (const FSlotAttribute* InBufSlotAttribute = InPlayerState->GetSlotAttributeTemplate(InSlotAttribute.BuffID))
			{
				InSlotAttribute.Buff = InBufSlotAttribute;
			}
		}

		if (InSlotAttribute.SkillID != INDEX_NONE)
		{
			if (const FSlotAttribute* InSkillSlotAttribute = InPlayerState->GetSlotAttributeTemplate(InSlotAttribute.SkillID))
			{
				InSlotAttribute.Skill = InSkillSlotAttribute;
			}
		}
	}
	

	void TipRegistration(UUI_Tip* Tip, const FSlotAttribute* InSlotAttribute)
	{
		//搜寻 基础
		{
			FText BaseString;
			GetBaseAttributeDescription(InSlotAttribute, BaseString);
			Tip->SetTextAction(BaseString);
		}

		//搜寻 被动
		{
			FText BaseString;
			GetBaseAttributeDescription(InSlotAttribute->Buff, BaseString);
			Tip->SetTextPassive(BaseString);
		}

		//搜寻 主动
		{
			FText BaseString;
			GetBaseAttributeDescription(InSlotAttribute->Skill, BaseString);
			Tip->SetTextActive(BaseString);
		}
	}

}
#undef  LOCTEXT_NAMESPACE