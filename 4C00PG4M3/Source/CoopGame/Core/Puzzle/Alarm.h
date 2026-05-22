// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Alarm.generated.h"

class URectLightComponent;

class UAudioComponent;

UCLASS()
class COOPGAME_API AAlarm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAlarm();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URectLightComponent* RectLight;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAudioComponent* Audio;

private:
	UFUNCTION()
	void HandleAlarmChanged(bool bNewState);

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* MaterialLightAlarmCos; 

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* MaterialLightAlarmSin;
};
