#include "RenderingUtils.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "Landscape.h"
#include "Engine/SceneCapture2D.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif
namespace RenderingUtils
{
	FScreenShot::FScreenShot(
		int32 InWidth,
		int32 InHeight,
		UTexture2D*& InTexture,
		UObject* InOuter,
		int32 InImageQuality /*= 80*/,
		bool bInShowUI /*= false*/,
		bool bAddFilenameSuffix /*= true*/)
		:Texture(InTexture)
		, ScaledWidth(InWidth)
		, ScaledHeight(InHeight)
		, ImageQuality(InImageQuality)
		, Outer(InOuter)
	{
		if (!UGameViewportClient::OnScreenshotCaptured().IsBound())
		{
			Filename = FPaths::ProjectSavedDir() / TEXT("SaveGames") / FGuid::NewGuid().ToString();
			ScreenShotDelegateHandle = UGameViewportClient::OnScreenshotCaptured().AddRaw(
				this,
				&RenderingUtils::FScreenShot::OnScreenshotCapturedInternal);

			FScreenshotRequest::RequestScreenshot(Filename, bInShowUI, bAddFilenameSuffix);
			Filename += TEXT(".jpg");
		}
	}

	void FScreenShot::OnScreenshotCapturedInternal(
		int32 SrcWidth,
		int32 SrcHeight,
		const TArray<FColor>& OrigBitmap)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		check(OrigBitmap.Num() == SrcWidth * SrcHeight);

		// 调整图像大小以强制最大大小。 
		TArray<FColor> ScaledBitmap;
		FImageUtils::ImageResize(SrcWidth, SrcHeight, OrigBitmap, ScaledWidth, ScaledHeight, ScaledBitmap, true);

		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		ImageWrapper->SetRaw(ScaledBitmap.GetData(), ScaledBitmap.GetAllocatedSize(), ScaledWidth, ScaledHeight, ERGBFormat::BGRA, 8);

		//jpg资源包头
		const TArray64<uint8>& JPEGData = ImageWrapper->GetCompressed(ImageQuality);
		FFileHelper::SaveArrayToFile(JPEGData, *Filename);

		//压缩
		FCreateTexture2DParameters Params;
		Params.bDeferCompression = true;
		Texture = FImageUtils::CreateTexture2D(ScaledWidth, ScaledHeight, ScaledBitmap, Outer, FGuid::NewGuid().ToString(), RF_NoFlags, Params);

		UGameViewportClient::OnScreenshotCaptured().Remove(ScreenShotDelegateHandle);
		ImageWrapper.Reset();

		//结束自己
		delete this;
	}

	ASceneCapture2D* SpawnSceneCapture2D(UWorld* World, UClass* SceneCaptureClass, FVector2D& MapSize, float Life)
	{
		if (SceneCaptureClass)
		{
			//获取World中类型为ALandScape的所有迭代器
			for (TActorIterator<ALandscape> It(World); It; ++It)
			{
				if (ALandscape* BigMap = *It)
				{
					//都是正方形
					FVector BigMapSize = BigMap->GetActorScale3D();
					//MapSize = FVector2D(BigMapSize.X * 10, BigMapSize.Y * 10);
					MapSize = FVector2D(BigMapSize.X * 1000, BigMapSize.Y * 1000);

					FVector CenterPoint = FVector(MapSize.X / 2);

					if (ASceneCapture2D* NewCarma = World->SpawnActor<ASceneCapture2D>(SceneCaptureClass, CenterPoint, FRotator(-90.f, 0.f, 0.f)))
					{
						if (Life != 0.f)
						{
							NewCarma->SetLifeSpan(Life);
						}

						return NewCarma;
					}

					break;
				}
			}
		}

		return nullptr;
	}

	void ClientDeath(UWorld* NewWorld, bool bDeath)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(NewWorld, APostProcessVolume::StaticClass(), OutActors);

		for (auto& Tmp : OutActors)
		{
			if (APostProcessVolume* InVolume = Cast<APostProcessVolume>(Tmp))
			{
				if (InVolume->GetName() == "DeathPostProcessVolume")
				{
					if (InVolume->Settings.WeightedBlendables.Array.IsValidIndex(0))
					{
						FWeightedBlendable& InWeigeted = InVolume->Settings.WeightedBlendables.Array[0];
						UMaterialInstanceDynamic* PostMatDynamic = Cast<UMaterialInstanceDynamic>(InWeigeted.Object);
						if (!PostMatDynamic)
						{
							if (UMaterialInstance* PostMat = Cast<UMaterialInstance>(InWeigeted.Object))
							{
								PostMatDynamic = UMaterialInstanceDynamic::Create(PostMat, InVolume, NAME_None);
								InVolume->Settings.WeightedBlendables.Array[0].Object = PostMatDynamic;
							}
						}

						if (bDeath)
						{
							PostMatDynamic->SetScalarParameterValue("Death", 0.8f);
						}
						else
						{
							PostMatDynamic->SetScalarParameterValue("Death", 0.f);
						}
					}
				}
			}
		}
	}
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif
