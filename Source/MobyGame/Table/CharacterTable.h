#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/MobyGameTableBase.h"
#include "../MobyGameType.h"
#include "CharacterTable.generated.h"

USTRUCT(BlueprintType)
struct FSkill
{
	GENERATED_USTRUCT_BODY()

	FSkill();
	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	int32 ID;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	UAnimMontage* SkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	FText SkillIntroduce;

	//技能预提示
	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	UMaterialInterface* SkillTip;
};

//暴露到蓝图当成变量使用
//继承了FMobyGameTableBase里的CharacterID
USTRUCT(BlueprintType)
struct FCharacterTable : public FMobyGameTableBase
{
	GENERATED_USTRUCT_BODY()

	FCharacterTable();

	UPROPERTY(EditDefaultsOnly, Category = "Character Class")
	TSubclassOf<APawn> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class")
	ECharacterType CharacterType;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class")
	UTexture2D *CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	UTexture2D* MiniCharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class")
	FName CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class")
	TArray<UAnimMontage*> NormalAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class")
	UAnimMontage* GoHomeAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	FSkill QSkill;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	FSkill WSkill;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	FSkill ESkill;

	UPROPERTY(EditDefaultsOnly, Category = "Character Skill")
	FSkill RSkill;

	UPROPERTY(EditDefaultsOnly, Category = "Character Die Montage")
	TArray<UAnimMontage*> Death;

};