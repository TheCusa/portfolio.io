// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserSpawner.generated.h"


class UBoxComponent;
class ALaserHandler;

UCLASS()
class COOPGAME_API ALaserSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:	
	// Called every frame
private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxRef;
	//UPROPERTY()
	//ALaserHandler* LaserRef;

	UPROPERTY(EditAnywhere)
	FVector SpawnOffset;

	FVector SpawnerOrigin;
	FVector SpawnerBoxExtent;

	FVector LaserOrigin;
	FVector LaserBoxExtent;
	FVector LaserOffset;

	FRotator SpawnRotation;
	FActorSpawnParameters SpawnParams;

	UPROPERTY(EditInstanceOnly)
	FVector LaserDirection;

	UPROPERTY(EditInstanceOnly)
	float Speed;

	UPROPERTY(EditInstanceOnly)
	FRotator LaserRotation;

	UPROPERTY(EditAnywhere)
	FVector Offset;

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ALaserHandler> LaserClass;
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ALaserHandler> FakeLaserClass;
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ALaserHandler> SingleLaser;


	int LaserNumber = 7;
	UPROPERTY(EditAnywhere)
	float LaserTimer = 5.f;

	void SpawnCalculator();
	void SpawnLaser();
};
