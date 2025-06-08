// Fill out your copyright notice in the Description page of Project Settings.


#include "MobyGamePlayerState.h"
#include "Table/SlotTable.h"
#include "Table/SlotAttribute.h"
#include "Data/MisData.h"
#include "Table/CharacterTable.h"
#include "Common/MethodUnit.h"
#include "MobyPawn.h"
#include "MobyGame/Common/MethodUnit.h"
#include "Kismet/GameplayStatics.h"
#include "ThreadManage.h"
#include "Character/CharacterSpawnPoint.h"
#include "Character/CharacterInstance/MobyGamePlayerCharacter.h"
#include "Common/NumericalCalculationUnit.h"

AMobyGamePlayerState::AMobyGamePlayerState()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Slot_Table(TEXT("/Game/Table/SlotTable"));
	SlotTablePtr = Slot_Table.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Slot_Attribute(TEXT("/Game/Table/SlotAttribute"));
	SlotAttributePtr = Slot_Attribute.Object;

	PlayerData = CreateDefaultSubobject<UPlayerDataComponent>(TEXT("Data"));
	//RootComponent = PlayerData;

	GoldTime = 0.f;
	BuffTime = 0.f;
	BuffFlag = false;
	RecoveryTime = 0.f;
	ShopLocation = FVector::ZeroVector;
	BloodLocation = FVector::ZeroVector;

	bLockSkill = false;
}

void AMobyGamePlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	//��ȡ����Ӫ�̵�λ��
	TArray<AActor*> SpawnPointArrays;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawnPoint::StaticClass(), SpawnPointArrays);

	for (auto &Tmp : SpawnPointArrays)
	{
		if (ACharacterSpawnPoint *InShopSpawnPoint = Cast<ACharacterSpawnPoint>(Tmp))
		{
			if (InShopSpawnPoint->GetCharacterType() == ECharacterType::BOSS_SHOP)
			{
				if (InShopSpawnPoint->GetType() == PlayerData->Team)
				{
					ShopLocation = InShopSpawnPoint->GetActorLocation();
					break;
				}
			}
		}
	}

	if (GetWorld()->IsServer())
	{
		for (int32 i = 0; i < 6; i++)
		{
			RecursionCreateInventorySlot();
		}

		for (int32 i = 0; i < 4; i++)
		{
			RecursionCreateSkillSlot();
		}

		GThread::Get()->GetCoroutines().BindLambda(2.f, [&]()
		{			
			//����ҵ�CharacterAttribute���buff�ӳɣ�����ʵʱ���Լ���
			if (AMobyGameState *InGameState = MethodUnit::GetGameState(GetWorld()))
			{
				if (FCharacterAttribute* InSlotInstance = InGameState->GetCharacterAttribute(PlayerData->PlayerID))
				{
					InSlotInstance->SetBuff(PlayerData->SlotAttributes.ToSharedRef());
				}
			}

			//֪ͨ
			FSlotDataNetPackage NetInventoryPackage;
			GetInventorySlotNetPackage(NetInventoryPackage);
			ServerCallClientInitInventory(NetInventoryPackage);

			InitSlotSkill();

			//֪ͨ
			FSlotDataNetPackage NetSkillPackage;
			GetSkillSlotNetPackage(NetSkillPackage);
			ServerCallClientInitSkill(NetSkillPackage);


			//֪ͨ��������
			if (AMobyGameState *InGameState = MethodUnit::GetGameState(GetWorld()))
			{
				InGameState->RequestCharacterAttribute(GetPlayerData()->PlayerID, GetPlayerData()->PlayerID, ECharacterAttributeType::ATTR_NONE);
			}

			//��ʼ���ܵ�
			GetPlayerData()->SkillPoint = 10;

			//�ͻ�����ʾ���������� ��ʾ���ܼӵ����
			ShowSkillLv();
		});
	}

}

void AMobyGamePlayerState::TickData(float DeltaSeconds)
{
	//Super::Tick(DeltaSeconds);

	if (GetWorld()->IsServer())
	{
		Skill(DeltaSeconds);

		//����buffϵͳ
		Buff(DeltaSeconds);

		Gold(DeltaSeconds);

		Sale(DeltaSeconds);

		Recovery(DeltaSeconds);

		Assist(DeltaSeconds);
	}
}

//ÿһ�нṹ��FSlotTable
const FSlotTable* AMobyGamePlayerState::GetSlotTableTemplate(int32 InID)
{
	for (auto &Tmp : *GetSlotTablesTemplate())
	{
		if (Tmp->ID == InID)
		{
			return Tmp;
		}
	}

	return NULL;
}

const FSlotAttribute* AMobyGamePlayerState::GetSlotAttributeTemplate(int32 InID)
{
	for (auto& Tmp : *GetSlotAttributesTemplate())
	{
		if (Tmp->ID == InID)
		{
			return Tmp;
		}
	}

	return NULL;
}

const TArray<FSlotTable*>* AMobyGamePlayerState::GetSlotTablesTemplate()
{
	if (!CacheSlotTables.Num())
	{
		if (SlotTablePtr)
		{
			SlotTablePtr->GetAllRows(TEXT("Slot Tables"), CacheSlotTables);
		}
		else
		{

		}
	}
	return &CacheSlotTables;
}

const TArray<FSlotAttribute*>* AMobyGamePlayerState::GetSlotAttributesTemplate()
{
	if (!CacheSlotAttribute.Num())
	{
		if (SlotAttributePtr)
		{
			SlotAttributePtr->GetAllRows(TEXT("Slot Attribute"), CacheSlotAttribute);
		}
		else
		{

		}
	}
	return &CacheSlotAttribute;
}

FSlotAttribute* AMobyGamePlayerState::GetSlotAttribute(int32 InSlotID)
{
	if (PlayerData->SlotAttributes->Contains(InSlotID))
	{
		return (*PlayerData->SlotAttributes)[InSlotID];
	}

	return NULL;
}

bool AMobyGamePlayerState::IsCDValid(int32 InSlotID)
{
	if (FSlotData *InSlotData = GetSlotData(InSlotID))
	{
		return InSlotData->CD <= 0.f;
	}

	return false;
}

FSlotData* AMobyGamePlayerState::GetSlotData(int32 InSlotID)
{
	if (FSlotData* InInventorySlot = GetInventorySlotData(InSlotID))
	{
		return InInventorySlot;
	}
	else if (FSlotData* InSkillSlot = GetSkillSlotData(InSlotID))
	{
		return InSkillSlot;
	}

	return NULL;
}

bool AMobyGamePlayerState::AddSlotAttributes(int32 InKey, int32 InSlotID)
{
	//�ӱ�����SlotAttribute
	if (const FSlotAttribute *InSlotAttribute = GetSlotAttributeTemplate(InSlotID))
	{
		return AddSlotAttributes(InKey, InSlotAttribute);
	}

	return false;
}


bool AMobyGamePlayerState::AddSlotAttributes(int32 InKey, const FSlotAttribute* InPlayerSlotAttribute)
{
	if (InPlayerSlotAttribute)
	{
		if (PlayerData->SlotAttributes->Contains(InKey))
		{
			*(*PlayerData->SlotAttributes.Get())[InKey] = *InPlayerSlotAttribute;
		}
		else
		{
			PlayerData->SlotAttributes->Add(InKey, *InPlayerSlotAttribute);
		}

		//����ȥ���buff
		CallClientAddBuff(InPlayerSlotAttribute);

		//ע�� ��������������
		if (InPlayerSlotAttribute->AddLevelID != INDEX_NONE)
		{
			if (const FSlotAttribute* InSlotAttributeAddLevel = GetSlotAttributeTemplate(InPlayerSlotAttribute->AddLevelID))
			{
				(*PlayerData->SlotAttributes)[InKey]->AddLevelValue = InSlotAttributeAddLevel;
			}
		}

		//ע�� ����buff������ ��Ϊ���ܻ���buff�ĵ��߸�����buff����
		if (InPlayerSlotAttribute->BuffID != INDEX_NONE)
		{
			if (const FSlotAttribute* InSlotAttributeBuff = GetSlotAttributeTemplate(InPlayerSlotAttribute->BuffID))
			{
				(*PlayerData->SlotAttributes)[InKey]->Buff = InSlotAttributeBuff;
			}
		}

		//ע�� ����Skill������
		if (InPlayerSlotAttribute->SkillID != INDEX_NONE)
		{
			if (const FSlotAttribute* InSlotAttributeSkill = GetSlotAttributeTemplate(InPlayerSlotAttribute->SkillID))
			{
				(*PlayerData->SlotAttributes)[InKey]->Skill = InSlotAttributeSkill;
			}
		}

		return true;
	}

	return false;
}

//��û�õ�
bool AMobyGamePlayerState::RecursionAddSlotAttributes(int32 InSlotID)
{
	if (const FSlotAttribute* InSlotAttribute = GetSlotAttributeTemplate(InSlotID))
	{
		int32 RandID = FMath::RandRange(0, 9999999);
		if (!PlayerData->SlotAttributes->Contains(RandID))
		{
			PlayerData->SlotAttributes->Add(RandID, *InSlotAttribute);
			return true;
		}

		return RecursionAddSlotAttributes(InSlotID);
	}

	return false;
}

int32 AMobyGamePlayerState::GetItemAndCheckGold(const FSlotTable* InTable)
{
	int32 NewGold = 9999999;
	if (!InTable)
	{
		return NewGold;
	}

	//�õ����������
	NewGold = InTable->SlotGold;

	//I	  �ݹ��ȡ����
	TArray<int32> IteamIDs;
	RecursivelyGetItemID(InTable, IteamIDs);

	//II  �ռ������Ʒ����Ʒ�� 6
	TArray<const FSlotTable*>SlotTables;
	for (auto &ItemID : IteamIDs)
	{
		for (auto &Tmp : *GetInventorySlotData())
		{
			if (Tmp.Value.SlotID != INDEX_NONE)
			{
				if (const FSlotTable* InSlotTable = GetSlotTableTemplate(Tmp.Value.SlotID))
				{
					if (ItemID == InSlotTable->ID)
					{
						SlotTables.Add(InSlotTable);
						break;
					}
				}
			}
		}
	}

	//III ������
	for (auto &Tmp : SlotTables)
	{
		NewGold -= Tmp->SlotGold;
	}

	//�����ڷ�����
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		//I  �Ƴ���Ʒ������(ҲҪ���µ�������Ʒ��)
		for (auto &Tmp : SlotTables)
		{
			for (auto &TmpSlot : PlayerData->InventorySlots)//��Ʒ������
			{
				if (TmpSlot.Value.SlotID != INDEX_NONE)
				{
					if (TmpSlot.Value.SlotID == Tmp->ID)
					{
						TmpSlot.Value.Reset();
						PlayerData->SlotAttributes->Remove(TmpSlot.Key);

						ServerCallClientUpdateSlot(TmpSlot.Key, TmpSlot.Value);
						break;
					}
				}
			}
		}

		//II ���¿ͻ��������Ϣ(��ɫ����)
	}

	return NewGold;
}

void AMobyGamePlayerState::RecursivelyGetItemID(const FSlotTable* InTable, TArray<int32>& ItemIds)
{
	for (auto &Tmp : InTable->Childrens)
	{
		if (const FSlotTable *InSlotTable = GetSlotTableTemplate(Tmp))
		{
			if (InSlotTable->Childrens.Num() == 0)
			{
				ItemIds.Add(InSlotTable->ID);
			}
			else
			{
				RecursivelyGetItemID(InTable, ItemIds);
			}
		}
	}
}


bool AMobyGamePlayerState::AddSlotToInventory(int32 InSlotID)
{
	if (const FSlotTable *InSlotTable = GetSlotTableTemplate(InSlotID))
	{
		bool bExist = IsExistInInventory(InSlotID);
		bool bConsumables = InSlotTable->SlotType.Contains(ESlotType::SLOT_CONSUMABLES);

		auto AddSlot = [&]()->int32
		{
			for (auto& Tmp : PlayerData->InventorySlots)
			{
				if (Tmp.Value.SlotID == INDEX_NONE)
				{
					if (bConsumables && !bExist)
					{
						Tmp.Value.Number = 1;
					}
					Tmp.Value.SlotID = InSlotTable->ID;
					Tmp.Value.SlotICO = InSlotTable->SlotIcon;

					return Tmp.Key;
				}
			}

			return INDEX_NONE;
		};

		int32 InInventoryID = INDEX_NONE;
		if (bConsumables)
		{
			if (bExist)
			{
				for (auto& Tmp : PlayerData->InventorySlots)
				{
					if (InSlotTable->ID == Tmp.Value.SlotID && Tmp.Value.SlotID != INDEX_NONE)
					{
						if (Tmp.Value.Number < 5)
						{
							Tmp.Value.Number++;

							//֪ͨ�����ͻ��˸�����Slot
							ServerCallClientUpdateSlot(Tmp.Key, Tmp.Value);

							return true;
						}
					}
				}
			}
			else
			{
				InInventoryID = AddSlot();
			}
		}
		else
		{
			InInventoryID = AddSlot();
		}

		if (InInventoryID != INDEX_NONE)
		{
			bool bAttributes = true;
			if (!IsSlotAttributesContinued(InSlotID))
			{
				bAttributes = AddSlotAttributes(InInventoryID, InSlotID);

				//�Ǽ��ܲ�ͬ��
				if (!IsSlotAttributesSkill(InSlotID))
				{
					if (AMobyGameState* InState = GetWorld()->GetGameState<AMobyGameState>())
					{
						//���´�������� �������
						InState->RequestCharacterAttribute(
							GetPlayerData()->PlayerID,
							GetPlayerData()->PlayerID,
							ECharacterAttributeType::ATTR_NONE);
					}
				}
			}

			//֪ͨ�����ͻ��˸���Slot, �˴�����ô��������ڿͻ���ִ�У����»���Slot
			if (bAttributes)
			{
				ServerCallClientUpdateSlot(InInventoryID, PlayerData->InventorySlots[InInventoryID]);
			}

			return true;
		}
	}

	return false;
}

bool AMobyGamePlayerState::IsInventorySlotValid()
{
	for (auto &Tmp : PlayerData->InventorySlots)
	{
		if (Tmp.Value.SlotID == INDEX_NONE)
		{
			return true;
		}
	}

	return false;
}

bool AMobyGamePlayerState::IsInventorySlotValid(int32 InInventoryID)
{
	for (auto &Tmp : PlayerData->InventorySlots)
	{
		if (Tmp.Key == InInventoryID)
		{
			return Tmp.Value.SlotID != INDEX_NONE;
		}
	}

	return false;
}

TMap<int32, FSlotData>* AMobyGamePlayerState::GetInventorySlotData()
{
	return &PlayerData->InventorySlots;
}

FSlotData* AMobyGamePlayerState::GetInventorySlotData(int32 InInventoryID)
{
	if (PlayerData->InventorySlots.Contains(InInventoryID))
	{
		return &PlayerData->InventorySlots[InInventoryID];
	}

	return  NULL;
}

void AMobyGamePlayerState::GetInventoryAllKeys(TArray<int32>& InKeys)
{
	GetAllSlotKeys(PlayerData->InventorySlots, InKeys);
}

void AMobyGamePlayerState::CheckInventory(int32 InventoryID)
{
	if (FSlotData* InSlotData = GetInventorySlotData(InventoryID))
	{
		if 	(InSlotData->Number == 0)
		{
			InSlotData->Reset();
			PlayerData->SlotAttributes->Remove(InventoryID);
		}
	
	}
}

//���ݲ��洴��
void AMobyGamePlayerState::RecursionCreateInventorySlot()
{
	RecursionCreateSlot(PlayerData->InventorySlots, PlayerData->SkillSlots);
}

bool AMobyGamePlayerState::IsExistInInventory(int32 InItemID)
{	
	for (auto &Tmp : PlayerData->InventorySlots)
	{
		if (Tmp.Value.SlotID == InItemID)
		{
			if (Tmp.Value.Number < 5)
			{
				return true;
			}
		}
	}

	return false;
}

FSlotData* AMobyGamePlayerState::GetSkillSlotData(int32 InSkillID)
{
	if (PlayerData->SkillSlots.Contains(InSkillID))
	{
		return &PlayerData->SkillSlots[InSkillID];
	}

	return  NULL;
}

void AMobyGamePlayerState::RecursionCreateSkillSlot()
{
	RecursionCreateSlot(PlayerData->SkillSlots, PlayerData->InventorySlots);
}

void AMobyGamePlayerState::InitSlotSkill()
{
	if (const FCharacterTable *InCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), PlayerData->PlayerID))
	{
		auto AddSlotToSkill = [&](const FSkill& InSkill)
		{
			for (auto &Tmp : PlayerData->SkillSlots)
			{
				if (Tmp.Value.SlotID == INDEX_NONE)
				{
					Tmp.Value.SlotID = InSkill.ID;
					Tmp.Value.SlotICO = InSkill.Icon;
					Tmp.Value.Number = INDEX_NONE;
					Tmp.Value.CD = 0.f;

					//��Ӽ������ԣ��ڱ�������
					AddSlotAttributes(Tmp.Key, InSkill.ID);
					break;
				}
			}
		};

		AddSlotToSkill(InCharacterTable->QSkill);
		AddSlotToSkill(InCharacterTable->WSkill);
		AddSlotToSkill(InCharacterTable->ESkill);
		AddSlotToSkill(InCharacterTable->RSkill);
	}
}

void AMobyGamePlayerState::GetSkillAllKeys(TArray<int32>& InKeys)
{
	GetAllSlotKeys(PlayerData->SkillSlots, InKeys);
}

int32 AMobyGamePlayerState::GetSkillID(int32 InSlotID)
{
	for (auto &Tmp : PlayerData->SkillSlots)
	{
		if (Tmp.Key == InSlotID)
		{
			return Tmp.Value.SlotID;
		}
	}

	return INDEX_NONE;
}


void AMobyGamePlayerState::GetInventorySlotNetPackage(FSlotDataNetPackage& InNetPackage)
{
	for (auto &Tmp : PlayerData->InventorySlots)
	{
		InNetPackage.SlotIDs.Add(Tmp.Key);
		InNetPackage.Slots.Add(Tmp.Value);
	}
}

void AMobyGamePlayerState::GetSkillSlotNetPackage(FSlotDataNetPackage& InNetPackage)
{
	for (auto &Tmp : PlayerData->SkillSlots)
	{
		InNetPackage.SlotIDs.Add(Tmp.Key);
		InNetPackage.Slots.Add(Tmp.Value);
	}
}


void AMobyGamePlayerState::UpdateTargetInfo(const int64 &InPlayerID)
{
	//����ID
	FLookPlayerInfoNetPackage InfoNetPackage;
	InfoNetPackage.PlayerID = InPlayerID;

	TArray<FBuffNetPackage> InBuffNetPackage;

	//����Inventory
	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState *InPlayerState)->MethodUnit::EServerCallType
	{
		if (InPlayerState->GetPlayerData()->PlayerID == InPlayerID)
		{
			InPlayerState->GetInventorySlotNetPackage(InfoNetPackage.SlotDataNetPackage);//��ȡ���ݰ�

			//�õ�buf��
			InPlayerState->GetBuffInfo(InBuffNetPackage);
			return MethodUnit::EServerCallType::PROGRESS_COMPLETE;
		}
		return MethodUnit::EServerCallType::INPROGRESS;
	});

	ServerCallClientUpdateLookPlayerInfo(InfoNetPackage);

	if (InBuffNetPackage.Num())
	{
		ServerCallClientAddBuffs(InBuffNetPackage);
	}

	//�������� ���ú����ڷ������ϣ����GetWorld�ҵ���Ҳ�Ƿ�������GameState
	if (AMobyGameState* InState = GetWorld()->GetGameState<AMobyGameState>())
	{
		InState->RequestCharacterAttribute(PlayerData->PlayerID, InPlayerID, ECharacterAttributeType::ATTR_NONE);
	}
}

//����������
void AMobyGamePlayerState::ServerCallClientUpdateLookPlayerInfo_Implementation(const FLookPlayerInfoNetPackage& InInfoNetPackage)
{
	if (LookPlayerInfoDelegate.IsBound())
	{
		LookPlayerInfoDelegate.Execute(InInfoNetPackage);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](const FLookPlayerInfoNetPackage& InInfoNetPackage)
		{
			ServerCallClientUpdateLookPlayerInfo(InInfoNetPackage);
		}, InInfoNetPackage);
	}
}

void AMobyGamePlayerState::ServerCallClientUpdateHideOrShowUI_Implementation(bool bHideOrNot)
{
	if (HideOrShowTopPanelDelegate.IsBound())
	{
		HideOrShowTopPanelDelegate.Execute(bHideOrNot);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			ServerCallClientUpdateHideOrShowUI(bHideOrNot);
		});
	}
}

void AMobyGamePlayerState::Buy_Implementation(int32 InSlotID)
{
	if (const FSlotTable *InTable = GetSlotTableTemplate(InSlotID))
	{
		//����ϳ��ж�ʣ������������
		int32 NewGold = GetItemAndCheckGold(InTable);
		if (PlayerData->Gold >= NewGold)//�Ƿ����㹻��Ǯ
		{
			if (AddSlotToInventory(InSlotID))
			{
				PlayerData->Gold -= NewGold;
			}
		}
	}
}

void AMobyGamePlayerState::Sell_Implementation(int32 InSlotID, int32 InItemID)
{
	Transaction(InSlotID, InItemID, 0.5f);
}

void AMobyGamePlayerState::CancellationOfOrder_Implementation(int32 InSlotID, int32 InItemID)
{
	Transaction(InSlotID, InItemID);
}


void AMobyGamePlayerState::ResponseCharacterAttribute_Implementation(int32 InPlayerID, const ECharacterAttributeType Type, float InValue)
{
	if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
	{
		InGameState->ResponseCharacterAttribute(InPlayerID, Type, InValue);
	}
}


void AMobyGamePlayerState::ResponseCharacterAttributes_Implementation(int64 InPlayerID, const FCharacterAttribute& InAttr)
{
	if (AMobyGameState* InGameState = GetWorld()->GetGameState<AMobyGameState>())
	{
		InGameState->ResponseCharacterAttributes(InPlayerID, InAttr);
	}
}

void AMobyGamePlayerState::ServerCallClientUpdateKillMsg_Implementation(const FKillNetPackage& KillNetPackage)
{
	if (KillMsgDelegate.IsBound())
	{
		KillMsgDelegate.Execute(KillNetPackage);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](const FKillNetPackage& InKillNetPackage)
		{
			ServerCallClientUpdateKillMsg(InKillNetPackage);
		}, KillNetPackage);
	}
}

//��֤���ܺ���ƷID���ظ�
void AMobyGamePlayerState::RecursionCreateSlot(TMap<int32, FSlotData>& InCreate, const TMap<int32, FSlotData>& InCompare)
{
	//�������UIID
	int32 SlotID = FMath::RandRange(0, 9999999);

	if (!InCreate.Contains(SlotID) && !InCompare.Contains(SlotID))
	{
		InCreate.Add(SlotID, FSlotData());
	}
	else
	{
		RecursionCreateSlot(InCreate, InCompare);
	}
	
}

void AMobyGamePlayerState::GetAllSlotKeys(const TMap<int32, FSlotData>& InData, TArray<int32>& InIDs)
{
	for (auto &Tmp : InData)
	{
		InIDs.Add(Tmp.Key);
	}
}

void AMobyGamePlayerState::Transaction(int32 InSlotID, int32 InItemID, float Discount /*= 1.0f*/)
{
	if (const FSlotTable* InTable = GetSlotTableTemplate(InItemID))
	{
		if (FSlotData* InSlotData = GetInventorySlotData(InSlotID))
		{
			//�����ݲ���
			if (InSlotData->Number > 0)
			{
				InSlotData->Number--;
				CheckInventory(InSlotID);
			}
			else
			{
				InSlotData->Reset();
				PlayerData->SlotAttributes->Remove(InSlotID);

				if (AMobyGameState* InState = GetWorld()->GetGameState<AMobyGameState>())
				{
					//���´�������� �������
					InState->RequestCharacterAttribute(
						GetPlayerData()->PlayerID,
						GetPlayerData()->PlayerID,
						ECharacterAttributeType::ATTR_NONE);
				}
			}

			//ȫ���˻��˿�
			PlayerData->Gold += (InTable->SlotGold * Discount);

			//֪ͨ�ͻ��� ����UI
			ServerCallClientUpdateSlot(InSlotID, *InSlotData);
		}
	}
}

void AMobyGamePlayerState::AddAssist(const int64& InPlayerID)
{
	AssistSystem.AddAssist(InPlayerID);
}

const FAssistPlayer* AMobyGamePlayerState::GetLastAssist()
{
	return AssistSystem.GetLastAssist();
}

const TArray<FAssistPlayer>& AMobyGamePlayerState::GetAssists()
{
	return AssistSystem.GetAssists();
}

void AMobyGamePlayerState::Use_Implementation(int32 InUIID)
{
	FSlotData* InSlotData = GetInventorySlotData(InUIID);
	if (!InSlotData)
	{
		InSlotData = GetSkillSlotData(InUIID);
		if (InSlotData)//����
		{
			if (!bLockSkill) //������
			{
				if (FSlotAttribute* InSkillAttribute = GetSlotAttribute(InUIID))
				{
					if (!UseMana(InSkillAttribute))
					{
						return;
					}
				}
			}
			else
			{
				return;
			}
		}
	}

	if (InSlotData)
	{
		if (InSlotData->Number != INDEX_NONE)
		{
			if (InSlotData->Number > 0)
			{
				InSlotData->Number--;
				ServerCallClientUpdateSlot(InUIID, *InSlotData);
			}
		}

		//buff
		if (IsSlotAttributesContinued(InSlotData->SlotID))
		{
			//UI���buff��ʾ����
			AddSlotAttributes(InUIID, InSlotData->SlotID);
		}

		if (FSlotAttribute *InAttribute = GetSlotAttribute(InUIID))
		{
			if (InAttribute->Type != ESlotAttributeType::SLOTATTRIBUTETYPE_PERMANENT)
			{
				if (InAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
				{
					//����buff����
					BuffSlotQueue.Add(InUIID, *InAttribute);			//buff
				}
				else
				{
					InSlotData->CD = InAttribute->CD.Value;
					PlayerData->SkillSlotQueue.Add(InUIID, InSlotData);//����
				}

				ServerCallClientStartUpdateCD(InUIID, *InSlotData);

				//ִ�м���
				int32 InSkillID = GetSkillID(InUIID);
				if (InSkillID != INDEX_NONE)
				{
					if (AMobyPawn* MobyPawn = GetPawn<AMobyPawn>())
					{
						//������ǰ��¼��ܼ�
						if (AMobyGamePlayerCharacter* InCharacter = Cast<AMobyGamePlayerCharacter>(MobyPawn->GetControllerCharacter()))
						{
							InCharacter->bPressSkillKey = true;
						}

						MobyPawn->SkillAttack(InSkillID);
					}
				}
			}
		}
	}

}

bool AMobyGamePlayerState::UseMana(const FSlotAttribute* InSlotAttribute)
{
	if (AMobyGameState* InState = GetWorld()->GetGameState<AMobyGameState>())
	{
		if (FCharacterAttribute* InCharacterAttribute = InState->GetCharacterAttribute(PlayerData->PlayerID))
		{
			if (InCharacterAttribute->UseMana(InSlotAttribute->ConsumeMana))
			{
				//�������
				InState->RequestCharacterAttribute(PlayerData->PlayerID, PlayerData->PlayerID, ECharacterAttributeType::ATTR_MANA);

				//�㲥�Լ�������
				if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
				{
					InState->NetMulticastCharacterAttribute(InPawn->GetControllerCharacter(), InCharacterAttribute->GetManaPercentage(), ECharacterAttributeType::ATTR_MANA);
				}

				return true;
			}
		}
	}

	return false;
}

void AMobyGamePlayerState::ServerCallClientEndUpdateCD_Implementation(int32 InID, const FSlotData& InNetSlotData)
{
	if (FSlotData *InSlotData = GetSlotData(InID))
	{
		*InSlotData = InNetSlotData;
	}
	EndUpdateCDSlotDelegate.Broadcast(InID);
}

void AMobyGamePlayerState::ServerCallClientStartUpdateCD_Implementation(int32 InID, const FSlotData& InNetSlotData)
{
	if (FSlotData *InSlotData = GetSlotData(InID))
	{
		*InSlotData = InNetSlotData;
	}

	StartUpdateCDSlotDelegate.Broadcast(InID);
}

void AMobyGamePlayerState::ServerCallClientUpdateSlot_Implementation(int32 InID, const FSlotData& InNetSlotData)
{
	//���ݸ���
	if (FSlotData* InSlotData = GetSlotData(InID))
	{
		*InSlotData = InNetSlotData;
	}

	//��ʾ����
	UpdateSlotDelegate.Broadcast(InID);
}

//��ʼ��Inventory
void AMobyGamePlayerState::ServerCallClientInitInventory_Implementation(const FSlotDataNetPackage& InInventorySlots)
{
	//�ͻ���ģ���ʼ������֧��ͬ�������Ҫ�ֶ�ͬ��
	//�Ӵ�������ҵ�6��slots����Ϊ���棬�Է�����Ϊ׼
	//PlayerData->InventorySlots.Empty();
	//for (int32 i = 0; i < InInventorySlots.SlotIDs.Num(); i++)
	//{
	//	PlayerData->InventorySlots.Add(InInventorySlots.SlotIDs[i], InInventorySlots.Slots[i]);
	//} 
	
	for (int32 i = 0; i < InInventorySlots.SlotIDs.Num(); i++)
	{
		if (PlayerData->InventorySlots.Contains(InInventorySlots.SlotIDs[i]))
		{
			PlayerData->InventorySlots[InInventorySlots.SlotIDs[i]] = InInventorySlots.Slots[i];
		}
		else
		{
			PlayerData->InventorySlots.Add(InInventorySlots.SlotIDs[i], InInventorySlots.Slots[i]);
		}
	} 

	InitSlotsDelegate.Broadcast(InInventorySlots.SlotIDs);
}

//��ʼ��Skill
void AMobyGamePlayerState::ServerCallClientInitSkill_Implementation(const FSlotDataNetPackage& InSkillSlots)
{
	//�ͻ���ģ���ʼ������֧��ͬ�������Ҫ�ֶ�ͬ��
	PlayerData->SkillSlots.Empty();
	for (int32 i = 0; i < InSkillSlots.SlotIDs.Num(); i++)
	{
		PlayerData->SkillSlots.Add(InSkillSlots.SlotIDs[i], InSkillSlots.Slots[i]);
	}

	InitSlotsDelegate.Broadcast(InSkillSlots.SlotIDs);
}

void AMobyGamePlayerState::ServerCallClientUpLvSkill_Implementation(const FSkillUpNetPackage& InUpNetPackage)
{
	if (UpdateSkillLvDelegate.IsBound())
	{
		UpdateSkillLvDelegate.Execute(InUpNetPackage);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](const FSkillUpNetPackage& InNewUpNetPackage)
		{
			ServerCallClientUpLvSkill(InNewUpNetPackage);
		}, InUpNetPackage);
	}
}

//�ڷ������ϲ������ݽ���
void AMobyGamePlayerState::ClientCallServerUpdateInventory_Implementation(int32 InMoveInventoryID, int32 InMovedInventoryID)
{
	//����֤��������ܵ�����
	//InventoryID��Slot�۽���ʱ�Ѿ�����ò��ܽ�����SlotID��FSlotData��ʼ��Ϊ-1������Slotʱ����
	if (PlayerData->InventorySlots.Contains(InMoveInventoryID) && PlayerData->InventorySlots.Contains(InMoveInventoryID))
	{
		FSlotData& InventorySlotA = PlayerData->InventorySlots[InMoveInventoryID];
		FSlotData& InventorySlotB = PlayerData->InventorySlots[InMovedInventoryID];

		if (InventorySlotB.SlotID == INDEX_NONE)
		{
			//�������ƶ�
			InventorySlotB = InventorySlotA;
			InventorySlotA.Reset();

			//�����ƶ�
			if (PlayerData->SlotQueue.Contains(InMoveInventoryID))
			{
				PlayerData->SlotQueue.Remove(InMoveInventoryID);
				PlayerData->SlotQueue.Add(InMovedInventoryID, &InventorySlotB);
			}

			//�����ƶ�
			PlayerData->SlotAttributes->SetKeyToNewKey(InMoveInventoryID, InMovedInventoryID);
		}
		else
		{
			//���ݽ���
			FSlotData InventoryTmpSlot = InventorySlotA;
			InventorySlotA = InventorySlotB;
			InventorySlotB = InventoryTmpSlot;

			//���н���
			{
				if (PlayerData->SlotQueue.Contains(InMoveInventoryID))
				{
					PlayerData->SlotQueue.Remove(InMoveInventoryID);
					PlayerData->SlotQueue.Add(InMoveInventoryID, &InventorySlotA);
				}

				if (PlayerData->SlotQueue.Contains(InMovedInventoryID))
				{
					PlayerData->SlotQueue.Remove(InMovedInventoryID);
					PlayerData->SlotQueue.Add(InMovedInventoryID, &InventorySlotB);
				}
			}
			//���Խ���
			PlayerData->SlotAttributes->SwapKeyAKeyB(InMoveInventoryID, InMovedInventoryID);
		}

		//֪ͨ�����ͻ��˸���UI
		ServerCallClientUpdateSlot(InMoveInventoryID, InventorySlotA);
		ServerCallClientUpdateSlot(InMovedInventoryID, InventorySlotB);
	}
}

void AMobyGamePlayerState::Reborn()
{
	if (SimpleBornDelegate.IsBound())
	{
		SimpleBornDelegate.Execute();
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]() { Reborn(); });
	}
}

void AMobyGamePlayerState::GetPlayerKillInfo(FPlayerKillInfoNetPackage& InNetPackage)
{
	InNetPackage.Assists = GetPlayerData()->Assists;
	InNetPackage.Kill = GetPlayerData()->Kill;
	InNetPackage.Killed = GetPlayerData()->Killed;
	InNetPackage.KillDogFace = GetPlayerData()->KillDogFace;
}

void AMobyGamePlayerState::RequestAllCharacterTeamsAttribute_Implementation()
{
	TArray<FPlayerTeamNetPackage> TeamNetPackage;

	//�ռ���Ϣ
	if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
	{
		MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(), [&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
		{
			TeamNetPackage.Add(FPlayerTeamNetPackage());
			FPlayerTeamNetPackage& TeamNet = TeamNetPackage.Last();
			for (auto &Tmp : InPlayerState->GetPlayerData()->InventorySlots)
			{
				TeamNet.SlotDataNetPackage.SlotIDs.Add(Tmp.Key);
				TeamNet.SlotDataNetPackage.Slots.Add(Tmp.Value);
			}
		
			//�����Ϣ�ռ�
			InPlayerState->GetPlayerKillInfo(TeamNet.PlayerInfo.KillInfo);

			//�����Ϣ
			TeamNet.PlayerInfo.PlayerID = InPlayerState->GetPlayerData()->PlayerID;
			
			if (const FCharacterAttribute* CharacterAttribute = MethodUnit::GetCharacterAttributeByPlayerID(GetWorld(), InPlayerState->GetPlayerData()->PlayerID))
			{
				TeamNet.PlayerInfo.CharacterLevel = CharacterAttribute->Lv;
			}

			if (const FCharacterTable* InTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), InPlayerState->GetPlayerData()->PlayerID))
			{
				TeamNet.PlayerInfo.PlayerIcon = InTable->CharacterIcon;
			}

			//�ռ�������Ϣ
			if (AMobyPawn *InPawn = InPlayerState->GetPawn<AMobyPawn>())
			{
				if (AMobyGameCharacter *InCharacter = InPawn->GetControllerCharacter())
				{
					if (InCharacter->IsDie())
					{
						TeamNet.PlayerInfo.DeathTime = InGameState->GetRebornTime(InCharacter);
					}
				}
			}

			return MethodUnit::EServerCallType::INPROGRESS;
		});
	}

	ResponseAllCharacterTeamsAttribute(TeamNetPackage);
}

void AMobyGamePlayerState::ResponseAllCharacterTeamsAttribute_Implementation(const TArray<FPlayerTeamNetPackage>& InPackage)
{
	//Tab������
	if (SimplePlayerTeamDelegate.IsBound())
	{
		SimplePlayerTeamDelegate.Execute(InPackage);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](const TArray<FPlayerTeamNetPackage>& InPackage)
		{
			ResponseAllCharacterTeamsAttribute(InPackage);
		}, InPackage);
	}
}

void AMobyGamePlayerState::ServerCallClientUpdateKillInfo_Implementation(const FPlayerKillInfoNetPackage& InNetPackage)
{
	if (SimplePlayerKillInfoDelegate.IsBound())
	{
		SimplePlayerKillInfoDelegate.Execute(InNetPackage);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](const FPlayerKillInfoNetPackage& InNewNetPackage)
		{
			ServerCallClientUpdateKillInfo(InNewNetPackage);
		}, InNetPackage);
	}
}

void AMobyGamePlayerState::UpdateKillInfo()
{
	FPlayerKillInfoNetPackage InKillNetPackage;
	GetPlayerKillInfo(InKillNetPackage);
	ServerCallClientUpdateKillInfo(InKillNetPackage);
}


void AMobyGamePlayerState::ServerCallClientUpdateTeamKillInfo_Implementation(int32 InFriendly, int32 InEnemy)
{
	if (TeamKillInfoDelegate.IsBound())
	{
		TeamKillInfoDelegate.Execute(InFriendly, InEnemy);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](int32 InFriendlyNumber, int32 InEnemyNumber)
		{
			ServerCallClientUpdateTeamKillInfo(InFriendlyNumber, InEnemyNumber);
		}, InFriendly, InEnemy);
	}
}

void AMobyGamePlayerState::ReleaseSkill_Implementation()
{
	if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
	{
		//������ǰ��¼��ܼ�
		if (AMobyGamePlayerCharacter * InCharacter = Cast<AMobyGamePlayerCharacter>(InPawn->GetControllerCharacter()))
		{
			InCharacter->bPressSkillKey = false;
		}
	}
}

FSlotAttribute* AMobyGamePlayerState::GetSlotAttributeBySkillID(int32 InSkillID)
{
	for (auto &Tmp : PlayerData->SlotAttributes->Attributes)
	{
		if (Tmp.Value.ID == InSkillID)
		{
			return &Tmp.Value;
		}
	}

	return NULL;
}


FCharacterAttribute* AMobyGamePlayerState::GetCharacterAttribute()
{
	if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
	{
		return InGameState->GetCharacterAttribute(PlayerData->PlayerID);
	}

	return NULL;
}

bool AMobyGamePlayerState::IsSlotAttributesContinued(int32 InSlotID)
{
	if (const FSlotAttribute* InSlotAttribute = GetSlotAttributeTemplate(InSlotID))
	{
		return InSlotAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED;
	}

	return false;
}



bool AMobyGamePlayerState::IsSlotAttributesSkill(int32 InSlotID)
{
	if (const FSlotAttribute* InSlotAttribute = GetSlotAttributeTemplate(InSlotID))
	{
		return InSlotAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_SKILL;
	}

	return false;
}

void AMobyGamePlayerState::SetLockSkill(bool bCharacterLockSkill)
{
	bLockSkill = bCharacterLockSkill;

	ServerCallClientLockSkill(bLockSkill);
}

bool AMobyGamePlayerState::IsSkillTip(int32 InSlotID)
{
	//FSkill
	int32 SkillID = GetSkillID(InSlotID);
	if (SkillID != INDEX_NONE)
	{
		if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
		{
			if (const FSkill* InSkill = InGameState->GetSkillBySkillID(SkillID))
			{
				return InSkill->SkillTip != NULL;
			}
		}
	}

	return false;
}

void AMobyGamePlayerState::ShowAttackTip(int32 InSlotID, bool bShow)
{
	//FSkill
	int32 SkillID = GetSkillID(InSlotID);
	if (SkillID != INDEX_NONE)
	{
		if (AMobyPawn *InPawn = GetPawn<AMobyPawn>())
		{
			if (AMobyGamePlayerCharacter* InPlayerCharacter = InPawn->GetControllerCharacter())
			{
				if (bShow)
				{
					if (AMobyGameState* InGameState = GetWorld()->GetGameState<AMobyGameState>())
					{
						if (const FSkill* InSkill = InGameState->GetSkillBySkillID(SkillID))
						{
							InPlayerCharacter->SetSkillTip(InSkill->SkillTip);
						}
					}
				}
				else
				{
					InPlayerCharacter->SetSkillTip(NULL);
				}
			}
		}
	}
}

void AMobyGamePlayerState::ServerCallClientLockSkill_Implementation(bool bCharacterLockSkill)
{
	if (SimpleLockSkillDelegate.IsBound())
	{
		SimpleLockSkillDelegate.Execute(bCharacterLockSkill);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](bool bNewLockSkill)
		{
			ServerCallClientLockSkill(bNewLockSkill);
		}, bCharacterLockSkill);
	}
}

void AMobyGamePlayerState::AddBuff(const FSlotAttribute* InBuffAttribute, int32 InKeyID)
{
	if (InBuffAttribute)
	{
		if (InBuffAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
		{
			BuffSlotQueue.Add(InKeyID, *InBuffAttribute);//��Ӷ���

			//�㲥���ǵ�buff
			CallClientAddBuff(InBuffAttribute);
		}
	}
}

void AMobyGamePlayerState::AddBuff(const FSlotAttribute* InBuffAttribute, int64 InCasterPlayerID)
{
	if (InBuffAttribute)
	{
		if (InBuffAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
		{
			if (AMobyGamePlayerState* InCasterPlayerState = MethodUnit::GetPlayerState(GetWorld(), InCasterPlayerID))
			{
				int32 InKey = INDEX_NONE;
				if (InCasterPlayerState->GetSlotData(InBuffAttribute->ID, InKey))//��ʩ���ߵ�PlayerState���������
				{
					BuffSlotQueue.Add(InKey, *InBuffAttribute);//��Ӷ���

					//�㲥���ǵ�buff
					CallClientAddBuff(InBuffAttribute);
				}
			}
		}
	}
}

//��û�õ�
void AMobyGamePlayerState::AddSkill(FSlotAttribute* InSkillAttribute, int64 InCasterPlayerID)
{
	//��ʱ����ôд
	if (InSkillAttribute)
	{
		//������Ե����ǵĹ̶���ɫ����
		if (InSkillAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_SKILL)
		{
			if (AMobyGamePlayerState *InCasterPlayerState = MethodUnit::GetPlayerState(GetWorld(), InCasterPlayerID))
			{
				int32 InKey = INDEX_NONE;
				if (FSlotData *InSlotData = InCasterPlayerState->GetSlotData(InSkillAttribute->ID, InKey))
				{
					AddSlotAttributes(InKey, InSkillAttribute);
					PlayerData->SkillSlotQueue.Add(InKey, InSlotData);//��Ӷ���

					AddBuff(InSkillAttribute->Buff, InKey);
				}
			}
		}
	}
}

void AMobyGamePlayerState::CallClientAddBuff(const FSlotAttribute* InBuffAttribute)
{
	if (InBuffAttribute)
	{
		if (InBuffAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
		{
			if (AMobyGameState* InGameState = GetWorld()->GetGameState<AMobyGameState>())
			{
				InGameState->ServerCallClientAddBuff(PlayerData->PlayerID, InBuffAttribute->ID, InBuffAttribute->CD.Value);//���
			}
		}
	}
}

void AMobyGamePlayerState::UpdateSpawnPointLocation()
{
	//��ȡ�Լ���Ӫ���̵�λ�ú�Ѫ�ص�λ��
	TArray<AActor*> SpawnPointArrays;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSpawnPoint::StaticClass(), SpawnPointArrays);
	for (auto& Tmp : SpawnPointArrays)
	{
		if (ACharacterSpawnPoint* InShopSpawnPoint = Cast<ACharacterSpawnPoint>(Tmp))
		{
			if (InShopSpawnPoint->GetCharacterType() == ECharacterType::BOSS_SHOP)
			{
				if (InShopSpawnPoint->GetType() == PlayerData->Team)
				{
					ShopLocation = InShopSpawnPoint->GetActorLocation();
					ServerCallClientUpdateShopPos(ShopLocation);//�����ͻ���
				}
			}
			else if (InShopSpawnPoint->GetCharacterType() == ECharacterType::BLOOD_POOL)
			{
				if (InShopSpawnPoint->GetType() == PlayerData->Team)
				{
					BloodLocation = InShopSpawnPoint->GetActorLocation();
				}
			}
		}
	}
}

bool AMobyGamePlayerState::IsScopeStore()
{
	if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
	{
		if (AMobyGamePlayerCharacter* InCharacter = InPawn->GetControllerCharacter())
		{
			float Distance = FVector::Dist(InCharacter->GetActorLocation(), ShopLocation);
			if (Distance < 1000.f)
			{
				return true;
			}
		}
	}

	return false;

}

void AMobyGamePlayerState::ServerCallClientAddBuffs_Implementation(const TArray<FBuffNetPackage>& InBuffNetPackage)
{
	if (UpdateBuffInfosDelegate.IsBound())
	{
		UpdateBuffInfosDelegate.Execute(InBuffNetPackage); 
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](const TArray<FBuffNetPackage>& InBuffNetPackage_inst)
		{
			ServerCallClientAddBuffs(InBuffNetPackage_inst);
		}, InBuffNetPackage);
	}
}

void AMobyGamePlayerState::ServerCallClientUpdateShopPos_Implementation(const FVector &InLocation)
{
	ShopLocation = InLocation;
}


void AMobyGamePlayerState::GoHome_Implementation()
{
	if (AMobyPawn *InPawn = GetPawn<AMobyPawn>())
	{
		InPawn->GetControllerCharacter()->GoHome();
	}
}

void AMobyGamePlayerState::StartGoHomeAnim_Implementation()
{
	if (const FCharacterTable *InCharacterTable = MethodUnit::GetCharacterTableByPlayerID(GetWorld(), GetPlayerData()->PlayerID))
	{
		if (InCharacterTable->GoHomeAnim)
		{
			if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
			{
				if (AMobyGamePlayerCharacter* InCharacter = InPawn->GetControllerCharacter())
				{
					InCharacter->MultiCastPlayerAnimMontage(InCharacterTable->GoHomeAnim);
				}
			}
		}
	}
}


void AMobyGamePlayerState::EndGoHomeAnim_Implementation()
{
	if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
	{
		if (AMobyGamePlayerCharacter* InCharacter = InPawn->GetControllerCharacter())
		{
			InCharacter->MultiCastPlayerStopAnimMontage();
		}
	}
}

void AMobyGamePlayerState::GetBuffInfo(TArray<FBuffNetPackage> &InNetPakage)
{
	for (auto& Tmp : BuffSlotQueue)
	{
		if (FSlotAttribute* InAttribute = &Tmp.Value)
		{
			if (InAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
			{
				InNetPakage.Add(FBuffNetPackage());
				FBuffNetPackage& TmpNetPakage = InNetPakage.Last();
				TmpNetPakage.SlotID = InAttribute->ID;
				TmpNetPakage.MaxCD = InAttribute->CD.Value;
			}
		}
	}
}

FSlotData* AMobyGamePlayerState::GetSlotData(int32 InSlotID, int32 &InKey)
{
	//�ǲ���buff
	for (auto &Tmp : PlayerData->SlotAttributes->Attributes)
	{
		if (Tmp.Value.BuffID == InSlotID)
		{
			InKey = Tmp.Key;
			return GetSlotData(InKey);
		}
	}

	//�ǲ��Ǿ������
	for (auto& Tmp : PlayerData->SlotAttributes->Attributes)
	{
		if (Tmp.Value.ID == InSlotID)
		{
			InKey = Tmp.Key;
			return GetSlotData(InKey);
		}
	}

	return NULL;
}

void AMobyGamePlayerState::Buff(float DeltaSeconds)
{
	if (AMobyGameState* InGame = MethodUnit::GetGameState(GetWorld()))
	{
		if (AMobyPawn* InMobyPawn = GetPawn<AMobyPawn>())
		{
			// �������ڹ���
			{
				if (FCharacterAttribute* InCharacterAttribute = GetCharacterAttribute())
				{
					TArray<int32> RemoveSlots;
					for (auto& Tmp : BuffSlotQueue)
					{
						Tmp.Value.CD.Value -= DeltaSeconds;
						if (Tmp.Value.CD.Value <= 0.f || InCharacterAttribute->Health <= 0.f)
						{
							Tmp.Value.CD.Value = 0.f;

							if (FSlotAttribute* InAttribute = &Tmp.Value)
							{
								//buf ֻ��������Ʒ buf
								if (InAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
								{
									InGame->ServerCallClientEndBuff(PlayerData->PlayerID, InAttribute->ID);
								}

								//�������
								CheckInventory(Tmp.Key);

								//����
								/*ServerCallClientEndUpdateCD(Tmp.Key, *Tmp.Value);*/

								if (AMobyGameCharacter* InCharacter = InMobyPawn->GetControllerCharacter())
								{
									if (InAttribute->EffectType == ESkillEffectType::EFFECT_LIMIT)//����
									{
										InCharacter->SetLimit(false);
									}
									else if (InAttribute->EffectType == ESkillEffectType::EFFECT_SILENT)//��Ĭ
									{
										InCharacter->SetSilent(false);
									}
									else if (InAttribute->EffectType == ESkillEffectType::EFFECT_VERTIGO)//ѣ��
									{
										InCharacter->SetVertigo(false);
									}
								}
							}

							RemoveSlots.Add(Tmp.Key);
						}
					}

					for (auto& Tmp : RemoveSlots)
					{
						BuffSlotQueue.Remove(Tmp);
					}
				}
			}

			//
			//buffϵͳ���� ֻ����UpdateAttrValue Requestһ�²��ܽ���CharacterAttribute����
			{
				//����
				BuffTime += DeltaSeconds;
		
				auto UpdateAttrValue = [&](const FSlotAttributeValue& InValue, float& InNewValue, const ECharacterAttributeType Type) ->float
				{
					float LastValue = InNewValue;

					//����ֵ
					InNewValue = NumericalCalculationUnit::GetSlotAttributeValue(InValue, InNewValue);
					if (InNewValue != LastValue) //ֻ�б䶯�Ÿ���
					{
						InGame->RequestCharacterAttribute(
							PlayerData->PlayerID,
							PlayerData->PlayerID,
							Type);

						return InNewValue - LastValue;
					}

					return InNewValue - LastValue;
				};

				if (FCharacterAttribute* PlayerAttribute = MethodUnit::GetCharacterAttributeByPlayerID(GetWorld(), PlayerData->PlayerID))
				{
					if (AMobyGamePlayerCharacter* InPlayerCharacter = InMobyPawn->GetControllerCharacter())
					{
						auto ValueChange = [&](float InValueChange, float Ratio, const FLinearColor& InColorTrue, const FLinearColor& InColorFalse)
						{
							if (InValueChange != 0.f)
							{
								if (InValueChange > 0.f)
								{
									InMobyPawn->ServerCallClientSpawnDrawBuffValueText(InPlayerCharacter->GetActorLocation(), InValueChange, Ratio, InColorTrue);
								}
								else
								{
									InMobyPawn->ServerCallClientSpawnDrawBuffValueText(InPlayerCharacter->GetActorLocation(), InValueChange, Ratio, InColorFalse);
								}
							}
						};

						if (BuffTime > 1.f)
						{
							BuffTime = 0.f;
							for (auto& Tmp : BuffSlotQueue)
							{
								if (FSlotAttribute* InAttribute = &Tmp.Value)
								{
									if (InAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
									{
										float HelathValueChange = UpdateAttrValue(InAttribute->Health, PlayerAttribute->Health, ECharacterAttributeType::ATTR_HEALTH);
										ValueChange(HelathValueChange, 0.1f, FLinearColor::Green, FLinearColor::Red);

										float ManaValueChange = UpdateAttrValue(InAttribute->Mana, PlayerAttribute->Mana, ECharacterAttributeType::ATTR_MANA);
										ValueChange(ManaValueChange, 0.1f, FLinearColor::Blue, FLinearColor::Black);


										if (ManaValueChange != 0.f || HelathValueChange != 0.f)
										{
											//���½�ɫͷ�Ϲҵ�widget
											InPlayerCharacter->MultiCastWidgetInfo(PlayerAttribute->GetHealthPercentage(), PlayerAttribute->GetManaPercentage());
										}
									}
								}
							}
						}

						for (auto& Tmp : BuffSlotQueue)
						{
							if (FSlotAttribute* InAttribute = &Tmp.Value)
							{
								if (InAttribute->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
								{								
									if (!BuffFlag)
									{
										float ArmorValueChange = UpdateAttrValue(InAttribute->Armor, PlayerAttribute->Armor, ECharacterAttributeType::ATTR_ARMOR);
										ValueChange(ArmorValueChange, 0.1f, FLinearColor::White, FLinearColor::Black);

										BuffFlag = true;
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

void AMobyGamePlayerState::Skill(float DeltaSeconds)
{
	if (AMobyGameState* InGame = MethodUnit::GetGameState(GetWorld()))
	{
		if (AMobyPawn* InMobyPawn = GetPawn<AMobyPawn>())
		{
			// �������ڹ���		
			TArray<int32> RemoveSlots;
			for (auto& Tmp : PlayerData->SkillSlotQueue)
			{
				Tmp.Value->CD -= DeltaSeconds;
				if (Tmp.Value->CD <= 0.f)
				{
					Tmp.Value->CD = 0.f;
					if (const FSlotTable* InTable = GetSlotTableTemplate(Tmp.Value->SlotID))
					{

					}
					else if (0)
					{

					}

					if (FSlotAttribute* InAttribute = GetSlotAttribute(Tmp.Key))
					{
						//�������
						CheckInventory(Tmp.Key);

						//����
						ServerCallClientEndUpdateCD(Tmp.Key, *Tmp.Value);
					}

					RemoveSlots.Add(Tmp.Key);
				}
			}

			for (auto& Tmp : RemoveSlots)
			{
				PlayerData->SkillSlotQueue.Remove(Tmp);
			}
		}
	}
}

void AMobyGamePlayerState::Gold(float DeltaSeconds)
{
	//����㷨
	GoldTime += DeltaSeconds;
	if (GoldTime >= 1.0f)
	{
		GoldTime = 0.f;
		PlayerData->Gold += 2;
	}
}

void AMobyGamePlayerState::Sale(float DeltaSeconds)
{
	//��������
	float Distance = FVector::Dist(ShopLocation, GetPawn()->GetActorLocation());
	TArray<int32> RemoveSlots;
	for (auto& Tmp : PlayerData->SellSlotQueue)
	{
		if (Distance >= 1000.f)
		{
			RemoveSlots.Add(Tmp.Key);
			Tmp.Value->bCancelBuy = false;

			//����
			ServerCallClientUpdateSlot(Tmp.Key, *Tmp.Value);
		}
	}

	for (auto& Tmp : RemoveSlots)
	{
		PlayerData->SellSlotQueue.Remove(Tmp);
	}
}

void AMobyGamePlayerState::Recovery(float DeltaSeconds)
{
	if (AMobyPawn* InMobyPawn = GetPawn<AMobyPawn>())
	{
		if (AMobyGameState* InGame = MethodUnit::GetGameState(GetWorld()))
		{
			RecoveryTime += DeltaSeconds;
			if (RecoveryTime >= 1.0f)
			{
				RecoveryTime = 0.f;
				if (FCharacterAttribute* InPlayerAttribute = GetCharacterAttribute())
				{
					if (InPlayerAttribute->Health > 0.f) //������Ųſ��Իָ������������������
					{
						//I ���Ѫ��
						float TmpMana = 1.5f;
						float TmpHealth = 1.f;
						float Distance = FVector::Dist(InMobyPawn->GetControllerCharacter()->GetActorLocation(), BloodLocation);
						if (Distance < 1000.f)
						{
							TmpHealth = (InPlayerAttribute->GetMaxHealth() / 4.f) * (1.1f - Distance / 1000.f);
							TmpMana = (InPlayerAttribute->GetMaxMana() / 4.f) * (1.1f - Distance / 1000.f);
						}

						{
							//II �ָ��ӳ�
							if (InPlayerAttribute->Health + TmpHealth < InPlayerAttribute->MaxHealth)
							{
								InPlayerAttribute->Health += TmpHealth;


							}
							else
							{
								InPlayerAttribute->Health = InPlayerAttribute->MaxHealth;
							}

							//�㲥ͷ������
							InGame->NetMulticastCharacterAttribute(
								InMobyPawn->GetControllerCharacter(),
								InPlayerAttribute->GetHealthPercentage(),
								ECharacterAttributeType::ATTR_HEALTH);

							//���´��������
							InGame->RequestCharacterAttribute(
								GetPlayerData()->PlayerID,
								GetPlayerData()->PlayerID,
								ECharacterAttributeType::ATTR_HEALTH);
						}
					
						{
							if (InPlayerAttribute->Mana + TmpMana < InPlayerAttribute->MaxMana)
							{
								InPlayerAttribute->Mana += TmpMana;					
							}
							else
							{
								InPlayerAttribute->Mana = InPlayerAttribute->MaxMana;
							}

							//�㲥ͷ������
							InGame->NetMulticastCharacterAttribute(
								InMobyPawn->GetControllerCharacter(),
								InPlayerAttribute->GetManaPercentage(),
								ECharacterAttributeType::ATTR_MANA);

							//���´��������
							InGame->RequestCharacterAttribute(
								GetPlayerData()->PlayerID,
								GetPlayerData()->PlayerID,
								ECharacterAttributeType::ATTR_MANA);
						}				
					}
				}
			}
		}
	}
}

void AMobyGamePlayerState::Assist(float DeltaSeconds)
{
	AssistSystem.Tick(DeltaSeconds);
}

void AMobyGamePlayerState::ServerCallClientDie_Implementation(float InDeathTime)
{
	if (SimpleDieDelegate.IsBound())
	{
		SimpleDieDelegate.Execute(InDeathTime);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](float InNewDeathTime)
		{
			ServerCallClientDie(InNewDeathTime);
		}, InDeathTime);
	}
}

void AMobyGamePlayerState::ServerCallClientGameOver_Implementation(bool bVictory)
{
	if (GameOverDelegate.IsBound())
	{
		GameOverDelegate.Execute(bVictory);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](bool bNewVictory)
		{
			ServerCallClientGameOver(bNewVictory);
		}, bVictory);
	}
}

void AMobyGamePlayerState::ServerCallClientInitPlayerData_Implementation(const int64 &InPlayerID)
{
	PlayerData->PlayerID = InPlayerID;
}

//Server
void AMobyGamePlayerState::AddGold(int32 InNewGold)
{
	PlayerData->Gold += InNewGold;
	if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
	{
		FVector Location = InPawn->GetControllerCharacter()->GetActorLocation();
		ServerCallClientAddGold(InNewGold, Location);
	}
}

void AMobyGamePlayerState::ServerCallClientAddGold_Implementation(int32 InNewGold, const FVector& InLocation)
{
	if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
	{
		InPawn->ServerCallClientSpawnDrawGoldText(InLocation, InNewGold, 9.f, FColor::Yellow);
	}
}

//�ж���Щ�������������� ��������������������������� �����ͻ���
void AMobyGamePlayerState::ShowSkillLv()
{
	TArray<int32> LimitSlotIDs;
	for (auto &Tmp : PlayerData->SkillSlots)
	{
		if (FSlotAttribute *SkillAttribute = GetSlotAttribute(Tmp.Key))
		{
			//�ж��Ƿ��Ѿ�����
			if (SkillAttribute->Lv >= 3)
			{
				LimitSlotIDs.Add(Tmp.Key);
				continue;
			}
			else
			{
				if (SkillAttribute->LimitCondition.IsValidIndex(0))
				{
					int32 LimitLv = SkillAttribute->LimitCondition[0];
					if (SkillAttribute->Lv < LimitLv)
					{
						//�ȼ��ж�
						if (AMobyGameState* InState = GetWorld()->GetGameState<AMobyGameState>())
						{
							if (const FCharacterAttribute* CharacterAttr = InState->GetCharacterAttribute(PlayerData->PlayerID))
							{
								if (CharacterAttr->Lv < LimitLv)
								{
									LimitSlotIDs.Add(Tmp.Key);
								}
								
							}
							
						}

						continue;		
					}
				}
			}
		}
	}

	ShowSkillUp(LimitSlotIDs);
}

//RPC ServerCallClient
void AMobyGamePlayerState::ShowSkillUp_Implementation(const TArray<int32>& InUpNetPackage)
{
	if (ShowSkillUpDelegate.IsBound())
	{
		ShowSkillUpDelegate.Execute(InUpNetPackage);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f,
			[&](const TArray<int32>& InNewUpNetPackage)
		{
			ShowSkillUp(InNewUpNetPackage);
		}, InUpNetPackage);
	}
}

void AMobyGamePlayerState::AddSkillSlotPoint(int32 InSlotID)
{
	int32 InSkillID = GetSkillID(InSlotID);
	if (InSkillID != INDEX_NONE)
	{
		if (FSlotAttribute* InSkillData = GetSlotAttribute(InSkillID))
		{
			InSkillData->UpdateLv();
		}
	}
}

//RPC ClientCallServer
void AMobyGamePlayerState::UpdateSkillLv_Implementation(int32 InUIID)
{
	auto GetCharacterLv = [&]()->int32
	{
		if (AMobyGameState* InGameState = GetWorld()->GetGameState<AMobyGameState>())
		{
			if (FCharacterAttribute* InAttribute = InGameState->GetCharacterAttribute(GetPlayerData()->PlayerID))
			{
				return InAttribute->Lv;
			}
	
		}

		return INDEX_NONE;
	};

	FSkillUpNetPackage InUpNetPackage;

	//MobyGamePlayerCharacter.cpp��������playerstate�и���ʼ���ܵ�
	if (GetPlayerData()->SkillPoint >= 1)
	{
		if (FSlotAttribute* InSkillAttribute = GetSlotAttribute(InUIID))
		{
			if (InSkillAttribute->Lv < 3)
			{
				InSkillAttribute->UpdateLv();
				GetPlayerData()->SkillPoint--;

				//�ռ�
				InUpNetPackage.SlotID = InUIID;
				InUpNetPackage.Lv = InSkillAttribute->Lv;

				if (InSkillAttribute->LimitCondition.Num())
				{
					int32 LimitLv = InSkillAttribute->LimitCondition[0];
					int32 CharacterLv = GetCharacterLv();

					if (CharacterLv == INDEX_NONE)
					{
						return;
					}

					if (CharacterLv >= LimitLv)
					{
						//�Ƴ��ȼ�����
						InSkillAttribute->LimitCondition.Remove(LimitLv);
						if (GetPlayerData()->SkillPoint <= 0)
						{
							//������û�м��ܵ�,֪ͨ�ͻ���ȫ��������������
							InUpNetPackage.bHidAllSlot = true;
							InUpNetPackage.bEnableCurrentSlot = true;
						}
						else
						{
							if (InSkillAttribute->LimitCondition.IsValidIndex(0))
							{
								LimitLv = InSkillAttribute->LimitCondition[0];
								if (CharacterLv >= LimitLv)
								{
									//ȫ������ֻ���µ�ǰ��
									InUpNetPackage.bHidAllSlot = false;
									InUpNetPackage.bEnableCurrentSlot = true;
								}
								else
								{
									//�����ǰ�� ��������
									InUpNetPackage.bHidAllSlot = false;
									InUpNetPackage.bEnableCurrentSlot = false;
								}
							}
							else
							{
								//��ǰ�ļ��ܿ������
								InUpNetPackage.bHidAllSlot = false;
								InUpNetPackage.bEnableCurrentSlot = false;
							}
						}
					}
				}
				else
				{
					if (GetPlayerData()->SkillPoint <= 0)
					{
						InUpNetPackage.bHidAllSlot = true;
						InUpNetPackage.bEnableCurrentSlot = true;
					}
					else
					{
						InUpNetPackage.bHidAllSlot = false;
						InUpNetPackage.bEnableCurrentSlot = true;
					}
				}


				//������Call�ͻ���ִ����������
				ServerCallClientUpLvSkill(InUpNetPackage);
			}
		}
	}
}
