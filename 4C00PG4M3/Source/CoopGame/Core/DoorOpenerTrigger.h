// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorOpenerTrigger.generated.h"

class AMovingDoor;
class UBoxComponent;

UCLASS()
class COOPGAME_API ADoorOpenerTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OnBoxTriggerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Sets default values for this actor's properties
	ADoorOpenerTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TARGET DOOR REFERENCE")
	TArray<AMovingDoor*> TargetDoors;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider")
	UBoxComponent* BoxTrigger;
};
