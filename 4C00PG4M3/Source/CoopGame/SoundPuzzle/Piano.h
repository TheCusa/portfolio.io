// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Piano.generated.h"

class AGameplayGameMode;
class AMovingDoor;
class Utils;
class UMelodiesDataTable;
UCLASS()
class COOPGAME_API APiano : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APiano();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY()
	AGameplayGameMode* GameModeRef;
public:	
	void AddNote(Notes);

	TArray<Notes> NoteArray;

	UPROPERTY(EditAnywhere)
	float Timer;

	bool bIsPuzzleSolved = false;
		
	TArray<int8> GetPuzzleSolution();
	UPROPERTY(Replicated)
	TArray <Notes> PuzzleSolution;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<AMovingDoor> MovingDoor;

private:
	FTimerHandle TimerHandler;
	UPROPERTY(EditDefaultsOnly)
	UDataTable* MelodiesTable;
	
	void OnTimerFinished();
	void ResetStartTimer();
	TArray<Notes> PickRandomMelody();
	


};
