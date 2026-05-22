// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoopGame/FirstPuzzle/MovingDoor.h"
#include "LevelTransitionDoor.generated.h"

class UBoxComponent;
class ALevelStreamingManager;

/**
 * 
 */
UCLASS()
class COOPGAME_API ALevelTransitionDoor : public AMovingDoor
{
	GENERATED_BODY()

public:

	ALevelTransitionDoor();	
	
	UPROPERTY(EditInstanceOnly, Category="Level Streaming")
	TSoftObjectPtr<UWorld> LevelToLoad;

	UPROPERTY(EditInstanceOnly, Category="Level Streaming")
	TSoftObjectPtr<UWorld> LevelToUnload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level Streaming")
	ALevelStreamingManager* LevelStreamingManager;
	
	virtual void OpenDoor() override;

	UFUNCTION()
	void OnFarSideEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnFarSideExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider")
	UBoxComponent* FarSideTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider")
	UBoxComponent* OneWayBlocker;

	virtual void PlayEndTimelineSound() const override;

protected:
	virtual void BeginPlay() override;
	
};
