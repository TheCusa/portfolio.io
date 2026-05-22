// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoopGame/Core/Puzzle/LaserHandler.h"
#include "LaserRadial.generated.h"

/**
 * 
 */
class UBoxComponent;
UCLASS()
class COOPGAME_API ALaserRadial : public ALaserHandler
{
	GENERATED_BODY()
public:
	ALaserRadial();
protected:
	virtual void BeginPlay() override;
	virtual void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> ColliderHorizontal;
	UPROPERTY(EditDefaultsOnly, Category="Laser")
	TObjectPtr<UStaticMeshComponent> HorizontalLaserMesh;
};
