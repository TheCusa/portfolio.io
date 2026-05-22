// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayGameMode.generated.h"

class AHackerPlayerController;
class AAgentPlayerController;

UCLASS()
class COOPGAME_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameplayGameMode();
	void TickCountdown();
	void OnTimerExpired();
	FTimerHandle CountdownTimerHandle;
	
protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;
	
	virtual TSubclassOf<APlayerController> GetPlayerControllerClassToSpawnForSeamlessTravel(APlayerController* PreviousPlayerController) override;
	
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	void StartCountdown(int32 Duration);


	UPROPERTY(EditDefaultsOnly, Category = "Game Time", meta = (DisplayName = "Countdown"))
	int32 GameDuration = 60 * 60; // 5 minutes
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Spawns", meta = (DisplayName = "Hacker Data Asset"))
	class UCharacterDefinition* HackerCharacterDefinition;

	UPROPERTY(EditDefaultsOnly, Category = "Character Spawns", meta = (DisplayName = "Agent Data Asset"))
	class UCharacterDefinition* AgentCharacterDefinition;

	UPROPERTY(EditDefaultsOnly, Category = "Player Controller", meta = (DisplayName = "Hacker PlayerController BP"))
	TSubclassOf<AHackerPlayerController> HackerPlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "Player Controller", meta = (DisplayName = "Agent PlayerController BP"))
	TSubclassOf<AAgentPlayerController> AgentPlayerController;

private:
	int32 DevelopmentPlayerCounter = 0;
	TSubclassOf<APlayerController> GetPlayerControllerClassForIndex(int32 PlayerIndex);
	
	
	UPROPERTY(EditDefaultsOnly, Category = "DEBUG")
	bool bIsHackerServer;

};
