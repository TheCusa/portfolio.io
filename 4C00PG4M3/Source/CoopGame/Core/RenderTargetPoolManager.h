#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "RenderTargetPoolManager.generated.h"

UCLASS()
class COOPGAME_API ARenderTargetPoolManager : public AActor
{
	GENERATED_BODY()
	
public:
	ARenderTargetPoolManager();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RenderTarget Pool")
	int32 PoolSize = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RenderTarget Pool")
	FIntPoint TargetResolution = FIntPoint(1280, 720);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RenderTarget Pool")
	TEnumAsByte<ETextureRenderTargetFormat> TargetFormat = RTF_RGBA8;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RenderTarget Pool")
	TArray<UTextureRenderTarget2D*> RenderTargets;

	UTextureRenderTarget2D* GetRenderTarget(int32 Index) const;
};
