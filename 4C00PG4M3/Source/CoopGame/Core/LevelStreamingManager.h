// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamingManager.generated.h"

UCLASS()
class COOPGAME_API ALevelStreamingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelStreamingManager();
	void RequestLoad(const TSoftObjectPtr<UWorld>& LevelToLoad) const;
	void RequestUnload(const TSoftObjectPtr<UWorld>& LevelToUnload) ;
	UPROPERTY(Replicated)
	bool bHasInitializedCameraFeed = false;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	TSet<FName> LoadedLevels;

};
