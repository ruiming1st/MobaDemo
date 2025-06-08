// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_MiniMap.generated.h"

class UCanvasPanel;
class ASceneCapture2D;
class UImage;
class UMaterialInterface;
class UCanvasPanelSlot;
/**
 *
 */
UCLASS()
class MOBYGAME_API UUI_MiniMap : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MiniMap;

	UPROPERTY(meta = (BindWidget))
	UImage* MiniMapImage;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<ASceneCapture2D> CaptureClass;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TScriptInterface<UMaterialInterface> MiniMapMat;

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	bool IsExistence(const int64 InID);

	void ResetLocation(UCanvasPanelSlot* PanelSlot, const FVector2D& MinMapPos, const FVector2D& LocalSize2D);

protected:
	TMap<int64, TWeakObjectPtr<UCanvasPanelSlot>> CharacterIcons;

private:
	FVector2D MapSize;
};
