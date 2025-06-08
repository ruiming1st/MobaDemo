// Fill out your copyright notice in the Description page of Project Settings.

#include "MobyGameState.h"
#include "Net/UnrealNetwork.h"
#include "Common/MethodUnit.h"
#include "MobyGame/MobyGamePlayerState.h"
#include "ThreadManage.h"
#include "Character/CharacterSpawnPoint.h"
#include "Character/CharacterInstance/MobyGamePlayerCharacter.h"


AMobyGameState::AMobyGameState()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Character_Table(TEXT("/Game/Table/CharacterTable"));
	CharacterTablePtr = Character_Table.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Character_Attribute(TEXT("/Game/Table/CharacterAttribute"));
	CharacterAttributePtr = Character_Attribute.Object;

	bGameOver = false;
	bRedSpawnSuper = false;
	bBlueSpawnSuper = false;
}

void AMobyGameState::SetSpawnSuper(const ETeamType TeamType, bool bNewState)
{
	if (TeamType == ETeamType::TEAM_BLUE)
	{
		bBlueSpawnSuper = bNewState;
	}
	else if (TeamType == ETeamType::TEAM_RED)
	{
		bRedSpawnSuper = bNewState;
	}
}

const TArray<FCharacterTable*>* AMobyGameState::GetCharacterTablesTemplate()
{
	if (!CacheCharacterTables.Num())
	{
		if (CharacterTablePtr)
		{
			CharacterTablePtr->GetAllRows(TEXT("Character Table"), CacheCharacterTables);
		}
	}
	return &CacheCharacterTables;
}

const TArray<FCharacterAttribute*>* AMobyGameState::GetCharacterAttributesTemplate()
{
	if (!CacheCharacterAttribute.Num())
	{
		if (CharacterAttributePtr)
		{
			CharacterAttributePtr->GetAllRows(TEXT("Character Table"), CacheCharacterAttribute);
		}
	}
	return &CacheCharacterAttribute;
}

const FCharacterTable* AMobyGameState::GetCharacterTableTemplate(const int32 &InID)
{
	for (auto &Tmp : *GetCharacterTablesTemplate())
	{
		if (Tmp->ID == InID)
		{
			return Tmp;
		}
	}
	
	return NULL;
}

const FCharacterAttribute* AMobyGameState::GetCharacterAttributeTemplate(const int32 &InID)
{
	for (auto &Tmp : *GetCharacterAttributesTemplate())
	{
		if (Tmp->ID == InID)
		{
			return Tmp;
		}
	}

	return NULL;
}

const TMap<int64, FCharacterAttribute>* AMobyGameState::GetCharacterAttributes()
{
	return &CharacterAttributes;
}

FCharacterAttribute* AMobyGameState::GetCharacterAttribute(int64 InPlayerID)
{
	for (auto &Tmp : CharacterAttributes)
	{
		if (Tmp.Key == InPlayerID)
		{
			return &Tmp.Value;
		}
	}

	return NULL;
}

void AMobyGameState::UpdateCharacterAILocation(int64 InPlayerID, const FVector& InLocation)
{
	for (auto &Tmp : PlayerLocation)
	{
		if (Tmp.PlayerID == InPlayerID)
		{
			Tmp.Location = InLocation;
			break;
		}
	}
}

void AMobyGameState::AddCharacterAILocation(int64 InPlayerID, int32 InCharacterID, const FVector& InLocation)
{
	for (auto& Tmp : PlayerLocation)
	{
		if (Tmp.PlayerID == InPlayerID)
		{
			return;
		}
	}

	PlayerLocation.Add(FPlayerLocation());
	PlayerLocation[PlayerLocation.Num() - 1].CharacterID = InCharacterID;
	PlayerLocation[PlayerLocation.Num() - 1].PlayerID = InPlayerID;
	PlayerLocation[PlayerLocation.Num() - 1].Location = InLocation;
}

void AMobyGameState::AddCharacterData(int64 PlayerID, int32 CharacterID)
{
	if (!CharacterAttributes.Contains(PlayerID))
	{
		CharacterAttributes.Add(PlayerID, *GetCharacterAttributeTemplate(CharacterID));
		CharacterAttributes[PlayerID].Reset();
		
		//注册我们的玩家 添加等级项
		int32 AddLevelID = CharacterAttributes[PlayerID].AddLevelID;
		if (AddLevelID != INDEX_NONE)
		{
			if (AMobyGamePlayerState* InGamePlayerState = MethodUnit::GetPlayerState(GetWorld()))
			{
				if (const FSlotAttribute* InSlotAttribute = InGamePlayerState->GetSlotAttributeTemplate(AddLevelID))
				{
					CharacterAttributes[PlayerID].AddLevelValue = InSlotAttribute;
				}
			}
		}
	}
}

void AMobyGameState::RemoveCharacterData(int64 PlayerID)
{
	if (CharacterAttributes.Contains(PlayerID))
	{
		CharacterAttributes.Remove(PlayerID);
	}
}

bool AMobyGameState::GetCharacterAILocation(int64 InPlayerID, FVector& InLocation)
{
	for (auto &Tmp : PlayerLocation)
	{
		if (Tmp.CharacterID == InPlayerID)
		{
			InLocation = Tmp.Location;
			return true;
		}
	}

	return false;
}

int32 AMobyGameState::GetCharacterID(int64 InPlayerID)
{
	if (FCharacterAttribute* CA = GetCharacterAttribute(InPlayerID))
	{
		return CA->ID;
	}

	return INDEX_NONE;
}

const TArray<FPlayerLocation>& AMobyGameState::GetPlayerLocations()
{
	return PlayerLocation;
}

void AMobyGameState::RequestCharacterAttribute(int32 InPlayerID, int32 InUpdatePlayerID, const ECharacterAttributeType Type)
{
	if (InPlayerID != INDEX_NONE && InUpdatePlayerID != INDEX_NONE)
	{
		if (CharacterAttributes.Contains(InUpdatePlayerID))
		{
			const FCharacterAttribute& InCharacterAttribute = CharacterAttributes[InUpdatePlayerID];

			MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
			{
				if (InPlayerState->GetPlayerData()->PlayerID == InPlayerID)
				{
					//有消耗，可以进行优化
					FCharacterAttribute InAttr = InCharacterAttribute;
					InAttr.MaxMana = InCharacterAttribute.GetMaxMana();
					InAttr.MaxAnger = InCharacterAttribute.GetMaxAnger();
					InAttr.MaxHealth = InCharacterAttribute.GetMaxHealth();
					InAttr.PhysicalAttack = InCharacterAttribute.GetPhysicalAttack();
					InAttr.CriticalStrike = InCharacterAttribute.GetCriticalStrike();
					InAttr.Armor = InCharacterAttribute.GetArmor();
					InAttr.MagicAttack = InCharacterAttribute.GetMagicAttack();
					InAttr.MagicDefense = InCharacterAttribute.GetMagicDefense();
					InAttr.PhysicalPenetration = InCharacterAttribute.GetPhysicalPenetration();
					InAttr.MagicPenetration = InCharacterAttribute.GetMagicPenetration();
					InAttr.WalkSpeed = InCharacterAttribute.GetWalkSpeed();
					InAttr.AttackSpeed = InCharacterAttribute.GetAttackSpeed();

					switch (Type)
					{
					case ECharacterAttributeType::ATTR_NONE:
						InPlayerState->ResponseCharacterAttributes(InUpdatePlayerID, InAttr);
						break;
					case ECharacterAttributeType::ATTR_LV:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, (float)InAttr.Lv);
						break;
					case ECharacterAttributeType::ATTR_MANA:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.Mana);
						break;
					case ECharacterAttributeType::ATTR_MAX_MANA:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.MaxMana);
						break;
					case ECharacterAttributeType::ATTR_MAX_HEALTH:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.MaxHealth);
						break;
					case ECharacterAttributeType::ATTR_HEALTH:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.Health);
						break;
					case ECharacterAttributeType::ATTR_PHYSICAL_ATTACK:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.PhysicalAttack);
						break;
					case ECharacterAttributeType::ATTR_ARMOR:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.Armor);
						break;
					case ECharacterAttributeType::ATTR_MAGIC_ATTACK:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.MagicAttack);
						break;
					case ECharacterAttributeType::ATTR_MAGIC_DEFENSE:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.MagicDefense);
						break;
					case ECharacterAttributeType::ATTR_PHYSICAL_PENETRATION:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.PhysicalPenetration);
						break;
					case ECharacterAttributeType::ATTR_MAGIC_PENETRATION:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.MagicPenetration);
						break;
					case ECharacterAttributeType::ATTR_WALK_SPEED:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.WalkSpeed);
						break;
					case ECharacterAttributeType::ATTR_ATTACK_SPEED:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.AttackSpeed);
						break;
					case ECharacterAttributeType::ATTR_EXP:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.EmpircalValue);
						break;
					case ECharacterAttributeType::ATTR_MAX_EXP:
						InPlayerState->ResponseCharacterAttribute(InUpdatePlayerID, Type, InAttr.MaxEmpircalValue);
						break;
					}

					return MethodUnit::EServerCallType::PROGRESS_COMPLETE;
				}

				return MethodUnit::EServerCallType::INPROGRESS;
			});
		}
	}
}

void AMobyGameState::RemoveTurret(TWeakObjectPtr<AMobyGameArchitectureCharacter> InTurretsWeakPtr, const ETeamType InType)
{
	TurretsSystem.RemoveTurret(InTurretsWeakPtr, InType);
}

//客户端更新缓存池并通知UI更新, 更新的可能是自己的CharacterInfo面板也可能是他人的面板
void AMobyGameState::ResponseCharacterAttribute(int32 InPlayerID, const ECharacterAttributeType Type, float InValue)
{
	if (InPlayerID != INDEX_NONE)
	{
		if (!CharacterAttributes.Contains(InPlayerID))
		{
			CharacterAttributes.Add(InPlayerID, FCharacterAttribute());
		}

		if (FCharacterAttribute* InCharacterAttribute = &CharacterAttributes[InPlayerID])
		{
			CharacterAttributes.Contains(InPlayerID);
			switch (Type)
			{
			case ECharacterAttributeType::ATTR_LV:
				InCharacterAttribute->Lv = (int32)InValue;
				break;
			case ECharacterAttributeType::ATTR_MANA:
				InCharacterAttribute->Mana = InValue;
				break;
			case ECharacterAttributeType::ATTR_MAX_MANA:
				InCharacterAttribute->MaxMana = InValue;
				break;
			case ECharacterAttributeType::ATTR_MAX_HEALTH:
				InCharacterAttribute->MaxHealth = InValue;
				break;
			case ECharacterAttributeType::ATTR_HEALTH:
				InCharacterAttribute->Health = InValue;
				break;
			case ECharacterAttributeType::ATTR_PHYSICAL_ATTACK:
				InCharacterAttribute->PhysicalAttack = InValue;
				break;
			case ECharacterAttributeType::ATTR_ARMOR:
				InCharacterAttribute->Armor = InValue;
				break;
			case ECharacterAttributeType::ATTR_MAGIC_ATTACK:
				InCharacterAttribute->MagicAttack = InValue;
				break;
			case ECharacterAttributeType::ATTR_MAGIC_DEFENSE:
				InCharacterAttribute->MagicDefense = InValue;
				break;
			case ECharacterAttributeType::ATTR_PHYSICAL_PENETRATION:
				InCharacterAttribute->PhysicalPenetration = InValue;
				break;
			case ECharacterAttributeType::ATTR_MAGIC_PENETRATION:
				InCharacterAttribute->MagicPenetration = InValue;
				break;
			case ECharacterAttributeType::ATTR_WALK_SPEED:
				InCharacterAttribute->WalkSpeed = InValue;
				break;
			case ECharacterAttributeType::ATTR_ATTACK_SPEED:
				InCharacterAttribute->AttackSpeed = InValue;
				break;
			case ECharacterAttributeType::ATTR_MAX_EXP:
				InCharacterAttribute->MaxEmpircalValue = InValue;
				break;
			case ECharacterAttributeType::ATTR_EXP:
				InCharacterAttribute->EmpircalValue = InValue;
				break;
			}

			//通知客户端UI_Panel更新信息 
			SimpleUpdateAttributeDelegate.Broadcast(InPlayerID, Type);
		}
	}
}

//客户端响应
void AMobyGameState::ResponseCharacterAttributes(int64 InPlayerID, const FCharacterAttribute& InAttr)
{
	//缓存数据
	if (CharacterAttributes.Contains(InPlayerID))
	{
		CharacterAttributes[InPlayerID] = InAttr;
	}
	else
	{
		CharacterAttributes.Add(InPlayerID, InAttr);
	}

	SimpleUpdateAttributesDelegate.Broadcast(InPlayerID);
}

void AMobyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMobyGameState, PlayerLocation);
}

void AMobyGameState::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		TeamKillInfo.Add(ETeamType::TEAM_RED, 0);
		TeamKillInfo.Add(ETeamType::TEAM_BLUE, 0);

		GThread::Get()->GetCoroutines().BindLambda(2.f, [&]()
		{
			//绑定函数
			KillSystem.BloodFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				AchievementKill(InKillerPlayer, EKillType::BLOOD);
			};

			KillSystem.KillFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				Kill(InKillerPlayer, InKilledPlayer, EKillType::KILL);
			};

			KillSystem.DoubleKillsFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				SerialKill(InKillerPlayer, InKilledPlayer, EKillType::DOUBLEKILL);
			};

			KillSystem.TripleKillsFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				SerialKill(InKillerPlayer, InKilledPlayer, EKillType::TRIPLEKILL);
			};

			KillSystem.QuadraKillsFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				SerialKill(InKillerPlayer, InKilledPlayer, EKillType::QUADRAKILL);
			};

			KillSystem.PentaKillsFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				SerialKill(InKillerPlayer, InKilledPlayer, EKillType::PENTAKILL);
			};

			KillSystem.InvincibleFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				AchievementKill(InKillerPlayer, EKillType::INVINCIBLE);
			};

			KillSystem.LegendaryFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				AchievementKill(InKillerPlayer, EKillType::LEGENDARY);
			};

			KillSystem.AceFunction = [&](const int64& InKillerPlayer, const int64& InKilledPlayer)
			{
				bool bTeamDie = true;
				
				MethodUnit::ServerCallAllCharacterAI<AMobyGameCharacter>(GetWorld(), [&](AMobyGameCharacter* InCharacter)->MethodUnit::EServerCallType
				{
					if (!InCharacter->IsDie())
					{
						bTeamDie = false;
					}

					return MethodUnit::EServerCallType::INPROGRESS;
				});

				if (bTeamDie)
				{
					GThread::Get()->GetCoroutines().BindLambda(3.f, [&]()
					{
						AchievementKill(InKillerPlayer, EKillType::ACE);
					});
				}

			};
		});

		TArray<AActor*> SpawnPointArrays;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawnPoint::StaticClass(), SpawnPointArrays);

		TArray<ACharacterSpawnPoint*> CharacterSpawnPoints;

		for (auto &Tmp : SpawnPointArrays)
		{
			if (ACharacterSpawnPoint* InSpawnPoint = Cast<ACharacterSpawnPoint>(Tmp))
			{
				CharacterSpawnPoints.Add(InSpawnPoint);
			}
		}
		//初始化小兵生成器
		MinionsSystem.Init(CharacterSpawnPoints);
		MinionsSystem.SetWorld(GetWorld());

		//初始化野怪生成器
		MonsterSystem.Init(CharacterSpawnPoints);
		MonsterSystem.SetWorld(GetWorld());

		//初始化玩家生成器
		PlayerSystem.Init(CharacterSpawnPoints);
		PlayerSystem.SetWorld(GetWorld());

		//初始化塔的生成器
		TurretsSystem.Init(CharacterSpawnPoints);
		TurretsSystem.SetWorld(GetWorld());
	}
}

void AMobyGameState::SettlementDeath(const int64& InKillPlayerID, const int64& InKilledPlayerID)
{
	if (InKillPlayerID == InKilledPlayerID)
	{
		return;
	}

	struct FExp
	{
		FExp()
			:KillTeam(ETeamType::TEAM_NEUTRAL)
			, KilledLocation(FVector::ZeroVector)
			, KillLocation(FVector::ZeroVector)
			, EmpircalReward(0.f)
		{}

		ETeamType KillTeam;
		FVector KilledLocation;
		FVector KillLocation;
		float EmpircalReward;
	};

	FExp Exp;
	TArray<AMobyGamePlayerCharacter*> MobyGamePlayerCharacters;

	auto GetRolesInScope = [&]()
	{
		for (TActorIterator<AMobyGamePlayerCharacter> It(GetWorld(), AMobyGamePlayerCharacter::StaticClass()); It; ++It)
		{
			AMobyGamePlayerCharacter* MobyGamePlayerCharacter = *It;
			float Distance = FVector::Dist(Exp.KilledLocation, MobyGamePlayerCharacter->GetActorLocation());
			if (Distance <= 1500.f)
			{
				if (MobyGamePlayerCharacter->GetTeam() == Exp.KillTeam)
				{
					MobyGamePlayerCharacters.Add(MobyGamePlayerCharacter);
				}
			}
		}
	};

	if (FCharacterAttribute* InKillCharacterAttribute = GetCharacterAttribute(InKillPlayerID))
	{
		if (const FCharacterAttribute* InKilledCharacterAttribute = GetCharacterAttribute(InKilledPlayerID))
		{
			Exp.EmpircalReward = InKilledCharacterAttribute->GetEmpircalReward();

			if (IsPlayer(InKillPlayerID)) //击杀者是玩家
			{
				if (AMobyGamePlayerState* InKillPlayerState = MethodUnit::GetPlayerState(GetWorld(), InKillPlayerID))
				{
					//收集击杀者位置和队伍
					if (AMobyPawn *InKillPawn = InKillPlayerState->GetPawn<AMobyPawn>())
					{
						Exp.KillLocation = InKillPawn->GetControllerCharacter()->GetActorLocation();
						Exp.KillTeam = InKillPlayerState->GetPlayerData()->Team;
					}

					if (const FCharacterTable* InKillCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKillPlayerID))
					{
						//玩家杀玩家
						if (IsPlayer(InKilledPlayerID))
						{
							InKillPlayerState->GetPlayerData()->Kill++;

							InKillPlayerState->UpdateKillInfo();
							IterationTeamKillNumber(InKillPlayerState->GetPlayerData()->Team);

							if (AMobyGamePlayerState* InKilledPlayerState = MethodUnit::GetPlayerState(GetWorld(), InKilledPlayerID))
							{
								//收集被击杀者位置
								if (AMobyPawn* InKilledPawn = InKilledPlayerState->GetPawn<AMobyPawn>())
								{
									Exp.KilledLocation = InKilledPawn->GetControllerCharacter()->GetActorLocation();
								}


								InKilledPlayerState->GetPlayerData()->Killed++;
								InKilledPlayerState->UpdateKillInfo();
								//计算助攻
								for (const auto &Tmp : InKilledPlayerState->GetAssists())
								{
									if (AMobyGamePlayerState* InAssistsPlayerState = MethodUnit::GetPlayerState(GetWorld(), Tmp.PlayerID))
									{
										if (InKillPlayerState != InAssistsPlayerState)
										{
											InAssistsPlayerState->AddGold((int32)((float)InKilledCharacterAttribute->GetGoldReward() * 0.16f));

											InAssistsPlayerState->GetPlayerData()->Assists++;
											//更新助攻玩家客户端的GameInfo
											InAssistsPlayerState->UpdateKillInfo();
										}
									}
								}
							}

							//玩家杀玩家
							KillSystem.Kill(InKillPlayerID, InKilledPlayerID);

						}
						else
						{
							//玩家杀怪物
							//是不是塔
							//玩家杀怪物 1是塔，击杀塔的玩家进行奖励，范围内拆塔团队奖励 2不是塔，记录击杀怪物数
							if (const FCharacterTable* InKilledCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKilledPlayerID))
							{
								if (AMobyGameCharacter* InKilledGameCharacter = MethodUnit::GetGameCharacterForce(GetWorld(), InKilledPlayerID))
								{
									Exp.KilledLocation = InKilledGameCharacter->GetActorLocation();

									//是不是塔
									if (InKilledCharacterTable->CharacterType >= ECharacterType::FIRST_CLASS_TURRETS && InKilledCharacterTable->CharacterType <= ECharacterType::BASE_TOWER)
									{
										//
										//InKillPlayerState->GetPlayerData()->Gold;

										//收集击杀信息
										FKillNetPackage NetPackage;
										NetPackage.KilledName = InKilledCharacterTable->CharacterName.ToString();
										NetPackage.KilledIcon = InKilledCharacterTable->CharacterIcon;
										NetPackage.KillerName = InKillPlayerState->GetPlayerData()->PlayerName.ToString();
										NetPackage.KillerIcon = InKillCharacterTable->CharacterIcon;

										UpdateKillMsg(NetPackage);
									}
									else
									{
										InKillPlayerState->GetPlayerData()->KillDogFace++;
										InKillPlayerState->UpdateKillInfo();
									}
								}
							}
						}

						InKillPlayerState->AddGold(InKilledCharacterAttribute->GetGoldReward());

					}
				}
			}
			else
			{
				if (const FCharacterTable* InKillerCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKillPlayerID))
				{
					//收集击杀方怪物信息
					if (AMobyGameCharacter* InKillGameCharacter = MethodUnit::GetGameCharacterForce(GetWorld(), InKillPlayerID))
					{
						Exp.KillLocation = InKillGameCharacter->GetActorLocation();
						Exp.KillTeam = InKillGameCharacter->GetTeam();
					}

					// 怪物杀玩家
					if (IsPlayer(InKilledPlayerID))
					{
						if (AMobyGamePlayerState* InKilledPlayerState = MethodUnit::GetPlayerState(GetWorld(), InKilledPlayerID))
						{
							//收集被击杀玩家位置和队伍
							if (AMobyPawn* InKilledPawn = InKilledPlayerState->GetPawn<AMobyPawn>())
							{
								Exp.KilledLocation = InKilledPawn->GetControllerCharacter()->GetActorLocation();
							}

							if (const FCharacterTable* InKilledCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKilledPlayerID))
							{
								InKilledPlayerState->GetPlayerData()->Killed++;
								InKilledPlayerState->UpdateKillInfo();
								//有主要助攻玩家
								if (const FAssistPlayer* InMainAssistPlayer = InKilledPlayerState->GetLastAssist())
								{
									if (AMobyGamePlayerState* InMainAssistPlayerState = MethodUnit::GetPlayerState(GetWorld(), InMainAssistPlayer->PlayerID))
									{
										InMainAssistPlayerState->GetPlayerData()->Kill++;
										InMainAssistPlayerState->UpdateKillInfo();

										//统计分数
										IterationTeamKillNumber(InMainAssistPlayerState->GetPlayerData()->Team);

										for (const auto &Tmp : InKilledPlayerState->GetAssists())
										{
											if (AMobyGamePlayerState* InAssistsPlayerState = MethodUnit::GetPlayerState(GetWorld(), Tmp.PlayerID))
											{
												if (InAssistsPlayerState != InMainAssistPlayerState)
												{
													InAssistsPlayerState->AddGold((int32)((float)InKilledCharacterAttribute->GetGoldReward() * 0.16f));

													InAssistsPlayerState->GetPlayerData()->Assists++;//所有参加助攻玩家记录助攻人数
													InAssistsPlayerState->UpdateKillInfo();
												}
											}
										}

										//玩家助攻杀掉玩家
										KillSystem.Kill(InMainAssistPlayerState->GetPlayerData()->PlayerID, InKilledPlayerID);

										//奖励金币
										InMainAssistPlayerState->AddGold(InKilledCharacterAttribute->GetGoldReward());
									}


								}
								else // 无助攻玩家
								{
									//没有助攻，玩家被杀
									if (InKillerCharacterTable->CharacterType >= ECharacterType::FIRST_CLASS_TURRETS &&
										InKillerCharacterTable->CharacterType <= ECharacterType::BASE_TOWER)
									{
										//if (const FAssistPlayer* InMainAssistPlayer = InKilledPlayerState->GetLastAssist())
										//{

										//}

									}

									//收集击杀信息
									FKillNetPackage NetPackage;
									NetPackage.KilledName = InKilledPlayerState->GetPlayerData()->PlayerName.ToString();
									NetPackage.KilledIcon = InKilledCharacterTable->CharacterIcon;
									NetPackage.KillerName = InKillerCharacterTable->CharacterName.ToString();
									NetPackage.KillerIcon = InKillerCharacterTable->CharacterIcon;

									UpdateKillMsg(NetPackage);
								}
							}
						}
					}
					else //怪物击杀怪物
					{
						if (const FCharacterTable* InKilledCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKilledPlayerID))
						{
							//收集被击杀方怪物信息
							if (AMobyGameCharacter* InKilledGameCharacter = MethodUnit::GetGameCharacterForce(GetWorld(), InKilledPlayerID))
							{
								Exp.KilledLocation = InKilledGameCharacter->GetActorLocation();

								//是不是塔
								if (InKilledCharacterTable->CharacterType >= ECharacterType::FIRST_CLASS_TURRETS && InKilledCharacterTable->CharacterType <= ECharacterType::BASE_TOWER)
								{
									if (AMobyGameTurretsCharacter *InTurretsCharacter = Cast<AMobyGameTurretsCharacter>(MethodUnit::GetGameCharacterForce(GetWorld(), InKilledPlayerID)))
									{
										if (const FAssistPlayer* InMainAssistPlayer = InTurretsCharacter->GetLastAssist())
										{
											if (AMobyGamePlayerState* InKillPlayerState = MethodUnit::GetPlayerState(GetWorld(), InMainAssistPlayer->PlayerID))
											{
												InKillPlayerState->AddGold(InKilledCharacterAttribute->GetGoldReward());
											}
										}
									}

									//收集击杀信息
									FKillNetPackage NetPackage;
									NetPackage.KilledName = InKilledCharacterTable->CharacterName.ToString();
									NetPackage.KilledIcon = InKilledCharacterTable->CharacterIcon;
									NetPackage.KillerName = InKillerCharacterTable->CharacterName.ToString();
									NetPackage.KillerIcon = InKillerCharacterTable->CharacterIcon;

									UpdateKillMsg(NetPackage);

									//金币奖励
									if (!MobyGamePlayerCharacters.Num())
									{
										//拿到范围内同队伍角色
										GetRolesInScope();
									}

									//范围内进行 拆塔 奖励
									for (auto &Tmp : MobyGamePlayerCharacters)
									{
										if (AMobyGamePlayerState* InPlayerStateInScope = MethodUnit::GetPlayerState(GetWorld(), Tmp->GetPlayerID()))
										{
											InPlayerStateInScope->AddGold(50);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	


	//作为经验奖励
	if (Exp.EmpircalReward != 0.f)
	{
		if (!MobyGamePlayerCharacters.Num())
		{
			GetRolesInScope();
		}

		if (MobyGamePlayerCharacters.Num())
		{
			float EmpircalReward = Exp.EmpircalReward / (float)MobyGamePlayerCharacters.Num();
			for (auto* Tmp : MobyGamePlayerCharacters)
			{
				if (!Tmp->IsDie())
				{
					Tmp->AddExp(EmpircalReward);
				}
			}
		}
	}
}

bool AMobyGameState::IsPlayer(const int64& InPlayerID)
{
	return MethodUnit::GetPlayerState(GetWorld(), InPlayerID) != NULL;
}

void AMobyGameState::Death(const int64& InPlayerID)
{
	//消除成就
	KillSystem.Death(InPlayerID);
}

float AMobyGameState::GetRebornTime(const AMobyGameCharacter* InCharacter)
{
	return PlayerSystem.GetRebornTime(InCharacter);
}

void AMobyGameState::GameOver(const ETeamType VictoryTeam)
{
	bGameOver = true;

	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(
		GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
		{
			if (VictoryTeam == InPlayerState->GetPlayerData()->Team)
			{
				//游戏成功
				InPlayerState->ServerCallClientGameOver(true);
			}
			else
			{
				//游戏失败
				InPlayerState->ServerCallClientGameOver(false);
			}

			return MethodUnit::EServerCallType::INPROGRESS;
		});
}

void AMobyGameState::UpdateKillMsg(const FKillNetPackage& InKillNetPackage)
{
	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
	{
		InPlayerState->ServerCallClientUpdateKillMsg(InKillNetPackage);
		return MethodUnit::EServerCallType::INPROGRESS;
	});
}

void AMobyGameState::Kill(const int64& InKillerPlayer, const int64& InKilledPlayer, EKillType InType)
{
	FKillNetPackage NetPackage;
	NetPackage.KillType = InType;
	if (const FCharacterTable* InCharacterKillerTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKillerPlayer))
	{
		NetPackage.KillerIcon = InCharacterKillerTable->CharacterIcon;
	}

	if (const FCharacterTable* InCharacterKilledTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKilledPlayer))
	{
		NetPackage.KilledIcon = InCharacterKilledTable->CharacterIcon;
	}

	if (AMobyGamePlayerState* InKillerGamePlayerState = MethodUnit::GetPlayerState(GetWorld(), InKillerPlayer))
	{
		NetPackage.KillerName = InKillerGamePlayerState->GetPlayerData()->PlayerName.ToString();
	}

	if (AMobyGamePlayerState* InKilledGamePlayerState = MethodUnit::GetPlayerState(GetWorld(), InKilledPlayer))
	{
		NetPackage.KilledName = InKilledGamePlayerState->GetPlayerData()->PlayerName.ToString();
	}

	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
	{
		InPlayerState->ServerCallClientUpdateKillMsg(NetPackage);
		return MethodUnit::EServerCallType::INPROGRESS;
	});
}

void AMobyGameState::SerialKill(const int64& InKillerPlayer, const int64& InKilledPlayer, EKillType InType)
{
	FKillNetPackage NetPackage;
	NetPackage.KillType = InType;
	if (const FCharacterTable* InCharacterKillerTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKillerPlayer))
	{
		NetPackage.KillerIcon = InCharacterKillerTable->CharacterIcon;
	}

	if (const FCharacterTable* InCharacterKilledTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKilledPlayer))
	{
		NetPackage.KilledIcon = InCharacterKilledTable->CharacterIcon;
	}

	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
	{
		InPlayerState->ServerCallClientUpdateKillMsg(NetPackage);
		return MethodUnit::EServerCallType::INPROGRESS;
	});
}

void AMobyGameState::AchievementKill(const int64& InKillerPlayer, EKillType InType)
{
	FKillNetPackage NetPackage;
	NetPackage.KillType = InType;
	if (const FCharacterTable *InCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InKillerPlayer))
	{
		NetPackage.KillerIcon = InCharacterTable->CharacterIcon;
	}

	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
	{
		InPlayerState->ServerCallClientUpdateKillMsg(NetPackage);
		return MethodUnit::EServerCallType::INPROGRESS;
	});
}

void AMobyGameState::IterationTeamKillNumber(ETeamType Type)
{
	TeamKillInfo[Type]++;

	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
	{
		int32 InFriendly = 0;
		int32 InEnemy = 0;
		if (InPlayerState->GetPlayerData()->Team == ETeamType::TEAM_RED)
		{
			InFriendly = TeamKillInfo[ETeamType::TEAM_RED];
			InEnemy = TeamKillInfo[ETeamType::TEAM_BLUE];
		}
		else if (InPlayerState->GetPlayerData()->Team == ETeamType::TEAM_BLUE)
		{
			InFriendly = TeamKillInfo[ETeamType::TEAM_BLUE];
			InEnemy = TeamKillInfo[ETeamType::TEAM_RED];
		}

		InPlayerState->ServerCallClientUpdateTeamKillInfo(InFriendly, InEnemy);
		
		return MethodUnit::EServerCallType::INPROGRESS;
	});
}

int32 AMobyGameState::GetCharacterSkillAttributeID(const UAnimMontage* InAnimMontage)
{
	for (auto& Tmp : *GetCharacterTablesTemplate())
	{
		if (Tmp->QSkill.SkillMontage == InAnimMontage)
		{
			return Tmp->QSkill.ID;
		}
		else if (Tmp->WSkill.SkillMontage == InAnimMontage)
		{
			return Tmp->WSkill.ID;
		}
		else if (Tmp->ESkill.SkillMontage == InAnimMontage)
		{
			return Tmp->ESkill.ID;
		}
		else if (Tmp->RSkill.SkillMontage == InAnimMontage)
		{
			return Tmp->RSkill.ID;
		}
	}

	return INDEX_NONE;
}

const FCharacterTable* AMobyGameState::GetCharacterTableBySkillID(const int32 InSkillID)
{
	for (auto& Tmp : *GetCharacterTablesTemplate())
	{
		if (Tmp->QSkill.ID == InSkillID)
		{
			return Tmp;
		}
		else if (Tmp->WSkill.ID == InSkillID)
		{
			return Tmp;
		}
		else if (Tmp->ESkill.ID == InSkillID)
		{
			return Tmp;
		}
		else if (Tmp->RSkill.ID == InSkillID)
		{
			return Tmp;
		}
	}

	return NULL;
}

const FSkill* AMobyGameState::GetSkillBySkillID(const int32 InSkillID)
{
	for (auto& Tmp : *GetCharacterTablesTemplate())
	{
		if (Tmp->QSkill.ID == InSkillID)
		{
			return &Tmp->QSkill;
		}
		else if (Tmp->WSkill.ID == InSkillID)
		{
			return &Tmp->WSkill;
		}
		else if (Tmp->ESkill.ID == InSkillID)
		{
			return &Tmp->ESkill;
		}
		else if (Tmp->RSkill.ID == InSkillID)
		{
			return &Tmp->RSkill;
		}
	}

	return NULL;
}

void AMobyGameState::ServerCallClientAddBuff_Implementation(int64 InPlayerID, int32 InSlotID, float MaxCD)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		UpdateBuffInfoDelegate.Broadcast(InPlayerID, InSlotID, MaxCD);
	}
}

void AMobyGameState::ServerCallClientEndBuff_Implementation(int64 InPlayerID, int32 InSlotID)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		EndBuffInfoDelegate.Broadcast(InPlayerID, InSlotID);
	}
}

void AMobyGameState::NetMulticastCharacterAttribute_Implementation(APawn* InPawn, float InValue, const ECharacterAttributeType Type)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		if (AMobyGameCharacter* InGameCharacter = Cast<AMobyGameCharacter>(InPawn))
		{
			switch (Type)
			{
			case ECharacterAttributeType::ATTR_NONE:
				break;
			case ECharacterAttributeType::ATTR_LV:
				InGameCharacter->SetNewLv(InValue);
				break;
			case ECharacterAttributeType::ATTR_MANA:
				InGameCharacter->SetNewMana(InValue);
				break;
			case ECharacterAttributeType::ATTR_MAX_MANA:
				break;
			case ECharacterAttributeType::ATTR_MAX_HEALTH:
				break;
			case ECharacterAttributeType::ATTR_HEALTH:
				InGameCharacter->SetNewHealth(InValue);
				break;
			case ECharacterAttributeType::ATTR_PHYSICAL_ATTACK:
				break;
			case ECharacterAttributeType::ATTR_ARMOR:
				break;
			case ECharacterAttributeType::ATTR_MAGIC_ATTACK:
				break;
			case ECharacterAttributeType::ATTR_MAGIC_DEFENSE:
				break;
			case ECharacterAttributeType::ATTR_PHYSICAL_PENETRATION:
				break;
			case ECharacterAttributeType::ATTR_MAGIC_PENETRATION:
				break;
			case ECharacterAttributeType::ATTR_WALK_SPEED:
				break;
			case ECharacterAttributeType::ATTR_ATTACK_SPEED:
				break;
			case ECharacterAttributeType::ATTR_MAX_EXP:
				break;
			case ECharacterAttributeType::ATTR_EXP:
				break;
			}
		}
	}
}

void AMobyGameState::RemoveCharacterAILocation(int64 InPlayerID)
{
	FPlayerLocation PlayerLocation_Remove;
	for (auto &Tmp : PlayerLocation)
	{
		if (Tmp.PlayerID == InPlayerID)
		{
			PlayerLocation_Remove = Tmp;
		}
	}

	PlayerLocation.Remove(PlayerLocation_Remove);
}
