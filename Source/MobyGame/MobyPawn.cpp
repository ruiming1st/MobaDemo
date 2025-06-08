// Fill out your copyright notice in the Description page of Project Settings.

#include "MobyPawn.h"
#include "Character/CharacterInstance/MobyGamePlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Common/MethodUnit.h"
#include "MobyGameState.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "MobyGamePlayerState.h"
#include "PlayerComponent/PlayerDataComponent.h"
#include "../../SimpleThread/Source/SimpleThread/Public/ThreadManage.h"
#include "Character/AI/AIController/Core/MobyGameAIController.h"
#include "Actor/DrawText.h"
#include "SimpleDrawText.h"

// Sets default values
AMobyPawn::AMobyPawn()
{
	//copied from MonyGameCharacter.cpp
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	RootBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RootBox"));
	RootComponent = RootBox;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bPressSpaceBar = false;
	PosTime = 0.f;
}

// Called when the game starts or when spawned
void AMobyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//只有服务器才能生成,文件放在服务端
	//根据ID获取角色的类

	//if (GetLocalRole() == ROLE_Authority) //服务器上执行
	//{
	//	//InitPlayer需要gamestate，然而state此时为空，需要将initplayer函数延时执行
	//	//InitPlayer();
	//	//两个客户端的角色由此产生，小兵在gamemode中生成
	//	GThread::Get()->GetCoroutines().BindUObject(0.1f, this, &AMobyPawn::InitPlayer);
	//}
		
	
}

void AMobyPawn::EndPlay(const EEndPlayReason::Type EEndPlayReason)
{
	Super::EndPlay(EEndPlayReason);

	if (GetLocalRole() == ROLE_Authority)
	{
		if (MobyGamePlayerCharacter)
		{
			MobyGamePlayerCharacter->Destroy();
			MobyGamePlayerCharacter = NULL;
		}
	}
}

void AMobyPawn::ServerCallSetMobyGameCharacter_Implementation(AMobyGamePlayerCharacter* InMobyGameCharacter)
{
	MobyGamePlayerCharacter = InMobyGameCharacter;

	if (MobyGamePlayerCharacter)
	{
		SetActorLocation(MobyGamePlayerCharacter->GetActorLocation());
	}
}

// Called every frame
void AMobyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CursorToWorld != nullptr)
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			PosTime += DeltaTime;
			if (PosTime >= 2.f)
			{
				PosTime = 0.f;
				CallServerClientViewportPostion(GetActorLocation());
			}

			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				FHitResult TraceHitResult;
				PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
				FVector CursorFV = TraceHitResult.ImpactNormal;
				FRotator CursorR = CursorFV.Rotation();
				CursorToWorld->SetWorldLocation(TraceHitResult.Location);
				CursorToWorld->SetWorldRotation(CursorR);
			}
		}		

		if (bPressSpaceBar)
		{
			SetActorLocation(GetCharacterLocation());
		}
	}
}

FVector AMobyPawn::GetCharacterLocation()
{
	if (MobyGamePlayerCharacter)
	{
		return MobyGamePlayerCharacter->GetActorLocation();
	}

	return FVector::ZeroVector;
}

// Called to bind functionality to input
void AMobyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("SpaceBar", IE_Pressed, this, &AMobyPawn::PressSpaceBar);
	InputComponent->BindAction("SpaceBar", IE_Released, this, &AMobyPawn::ReleaseSpaceBar);
}

void AMobyPawn::PressSpaceBar()
{
	bPressSpaceBar = true;
}

void AMobyPawn::ReleaseSpaceBar()
{
	bPressSpaceBar = false;
}

void AMobyPawn::CallServerClientViewportPostion_Implementation(const FVector &InNewPos)
{
	SetActorLocation(InNewPos);
}

void AMobyPawn::ServerCallClientSpawnDrawText_Implementation(const FVector& Location, float Value, float Percentage, const FLinearColor& Color)
{
	FSimpleDrawTextModule& DrawTextModule = FModuleManager::GetModuleChecked<FSimpleDrawTextModule>("SimpleDrawText");
	if (ADrawText* InDrawText = GetWorld()->SpawnActor<ADrawText>(DrawTextModule.GetDrawTextClass(), Location, FRotator::ZeroRotator))
	{
		FString DrawTextString;
		if (Value > 0)
		{
			DrawTextString = FString::Printf(TEXT("- %.2f"), Value);
		}
		else
		{
			DrawTextString = FString::Printf(TEXT("+ %.2f"), Value);
		}
	
		InDrawText->SetTextBlock(DrawTextString, Color, Percentage);
	}
}

//使用插件绘制
void AMobyPawn::ServerCallClientSpawnDrawGoldText_Implementation(const FVector& Location, int32 Value, float Percentage, const FLinearColor& Color)
{
	FSimpleDrawTextModule& DrawTextModule = FModuleManager::GetModuleChecked<FSimpleDrawTextModule>("SimpleDrawText");
	if (ADrawText* InDrawText = GetWorld()->SpawnActor<ADrawText>(DrawTextModule.GetDrawTextClass(), Location, FRotator::ZeroRotator))
	{
		FString DrawTextString = FString::Printf(TEXT("+ %d"), Value);
		InDrawText->SetTextBlock(
			DrawTextString,
			Color,
			Percentage,
			EInfoAnimType::TYPE_DEFAULT,
			0);
	}
}

void AMobyPawn::ServerCallClientSpawnDrawBuffValueText_Implementation(const FVector& Location, float Value, float Percentage, const FLinearColor& Color)
{
	FSimpleDrawTextModule& DrawTextModule = FModuleManager::GetModuleChecked<FSimpleDrawTextModule>("SimpleDrawText");
	if (ADrawText* InDrawText = GetWorld()->SpawnActor<ADrawText>(DrawTextModule.GetDrawTextClass(), Location, FRotator::ZeroRotator))
	{
		FString DrawTextString;
		if (Value > 0)
		{
			DrawTextString = FString::Printf(TEXT("+ %.2f"), Value);
		}
		else
		{
			DrawTextString = FString::Printf(TEXT("- %.2f"), Value);
		}

		InDrawText->SetTextBlock(DrawTextString, Color, Percentage);
	}
}

void AMobyPawn::ServerCallClientSpawnDrawExpText_Implementation(const FVector& Location, int32 Value, float Percentage, const FLinearColor& Color)
{
	FSimpleDrawTextModule& DrawTextModule = FModuleManager::GetModuleChecked<FSimpleDrawTextModule>("SimpleDrawText");
	if (ADrawText* InDrawText = GetWorld()->SpawnActor<ADrawText>(DrawTextModule.GetDrawTextClass(), Location, FRotator::ZeroRotator))
	{
		FString DrawTextString = FString::Printf(TEXT("+ %d"), Value);
		InDrawText->SetTextBlock(
			DrawTextString,
			Color,
			Percentage,
			EInfoAnimType::TYPE_DEFAULT,
			0);
	}
}

void AMobyPawn::ServerCallClientSetPawnLocation_Implementation(const FVector& InLocation)
{
	SetActorLocation(InLocation);
}

void AMobyPawn::SkillAttack(int32 InSkillID)
{
	if (MobyGamePlayerCharacter)
	{
		MobyGamePlayerCharacter->SkillAttack(InSkillID);
	}
}

int64 AMobyPawn::GetPlayerID()
{
	return GetPlayerData()->PlayerID;
}

const UPlayerDataComponent* AMobyPawn::GetPlayerData()
{
	return GetPlayerState()->GetPlayerData();
}

AMobyGamePlayerState* AMobyPawn::GetPlayerState()
{
	return GetController()->GetPlayerState<AMobyGamePlayerState>();
}

//bool AMobyPawn::CharacterMoveToTargetAndAttackOnServer_Validate(const FVector &DirectionLocation, const APawn *InPawn)
//{
//	return InPawn != NULL && InPawn != MobyGameCharacter;
//}


void AMobyPawn::CharacterMoveToTargetAndAttackOnServer_Implementation(const FVector &DirectionLocation, const APawn *InPawn)
{
	if (AMobyGameCharacter* InTarget = Cast<AMobyGameCharacter>(const_cast<APawn*>(InPawn)))
	{
		if (MobyGamePlayerCharacter->GetTeam() != InTarget->GetTeam())
		{
			if (AMobyGameAIController* AGAIController = Cast<AMobyGameAIController>(MobyGamePlayerCharacter->GetController()))
			{
				AGAIController->SetTargetForce(InTarget);
			}
		}
	}
}

void AMobyPawn::CharacterMoveToOnServer_Implementation(const FVector &DirectionLocation)
{
	if (MobyGamePlayerCharacter)
	{
		float const Distance = FVector::Dist(DirectionLocation, MobyGamePlayerCharacter->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			//UAIBlueprintHelperLibrary::SimpleMoveToLocation(MobyGameCharacter->GetController(), DirectionLocation);
			if (AMobyGameAIController* AGAIController = Cast<AMobyGameAIController>(MobyGamePlayerCharacter->GetController()))
			{
				AGAIController->SimpleMoveToLocation(DirectionLocation);
			}
		}
	}
}

