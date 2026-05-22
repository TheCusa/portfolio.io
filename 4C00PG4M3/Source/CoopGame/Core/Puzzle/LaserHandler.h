// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserHandler.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class COOPGAME_API ALaserHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UPROPERTY(EditDefaultsOnly, Category="Laser")
	TObjectPtr<UStaticMeshComponent> LaserMesh;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> Collider;

	//UPROPERTY()
	//ARespawnPoint* RespawnPointRef;

	UPROPERTY(EditDefaultsOnly)
	bool bIsReal;

	

	 void HandleCollision(AActor*, UPrimitiveComponent*);
};
