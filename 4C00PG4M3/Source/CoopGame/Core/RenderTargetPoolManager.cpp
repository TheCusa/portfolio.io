#include "RenderTargetPoolManager.h"
#include "Kismet/KismetRenderingLibrary.h"

ARenderTargetPoolManager::ARenderTargetPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARenderTargetPoolManager::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < PoolSize; ++i)
	{
		UTextureRenderTarget2D* NewRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
			GetWorld(),
			TargetResolution.X,
			TargetResolution.Y,
			TargetFormat
		);

		if (NewRenderTarget)
		{
			RenderTargets.Add(NewRenderTarget);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("RenderTargetPool created %d targets"), RenderTargets.Num());
}

UTextureRenderTarget2D* ARenderTargetPoolManager::GetRenderTarget(int32 Index) const
{
	if (RenderTargets.IsValidIndex(Index))
	{
		return RenderTargets[Index];
	}
	return nullptr;
}
