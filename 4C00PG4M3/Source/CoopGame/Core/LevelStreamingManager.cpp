// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelStreamingManager.h"
#include "Kismet/GameplayStatics.h"
#include"Net/UnrealNetwork.h"

// Sets default values
ALevelStreamingManager::ALevelStreamingManager()
{
	PrimaryActorTick.bCanEverTick = false;
	
}

void ALevelStreamingManager::RequestLoad(const TSoftObjectPtr<UWorld>& LevelToLoad) const
{
	if (!LevelToLoad.IsNull())
	{
		UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, LevelToLoad, true, false, FLatentActionInfo());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Level"));
	}
}

void ALevelStreamingManager::RequestUnload(const TSoftObjectPtr<UWorld>& LevelToUnload) 
{
	if (LevelToUnload.IsValid())
	{
		UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(this, LevelToUnload, FLatentActionInfo(), false);
		bHasInitializedCameraFeed = false;
	}
}
void ALevelStreamingManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALevelStreamingManager, bHasInitializedCameraFeed);
}

