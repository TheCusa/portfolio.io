// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorLight.generated.h"

class USpotLightComponent;

UCLASS()
class COOPGAME_API AFloorLight : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloorLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpotLightComponent> SpotLight;

	UFUNCTION()
	void HandleAlarmChanged(bool bNewState);
};
