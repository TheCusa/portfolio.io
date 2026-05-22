// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoopPlayerController.h"
#include "HackerPlayerController.generated.h"

class AHackerMonitor;
class ALevelStreamingManager;
/**
 * 
 */
UCLASS()
class COOPGAME_API AHackerPlayerController : public ACoopPlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<AHackerMonitor> HackerMonitor;

public:
	UFUNCTION(Client, Reliable)
	void ReceiveArrayCode(const TArray<int8>& Array, const GameUserWidget Widget);
	UFUNCTION(Server, Reliable)
	void Server_UpdateFeed();
	UPROPERTY()
	ALevelStreamingManager* StreamingManager;
};
