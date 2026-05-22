// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "RotatingMovementComponentInterp.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COOPGAME_API URotatingMovementComponentInterp : public URotatingMovementComponent
{
	GENERATED_BODY()

public:	
	URotatingMovementComponentInterp();
	UPROPERTY(EditAnywhere, Category="Networking")
	float InterpSpeed = 10.0f;
	
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
private:

	UPROPERTY(ReplicatedUsing=OnRep_TargetRotation)
	FRotator ReplicatedTargetRotation;
	
	FRotator ClientInterpolationTarget;

	
	UFUNCTION()
	void OnRep_TargetRotation();
};
