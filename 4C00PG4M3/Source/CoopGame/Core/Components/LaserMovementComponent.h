// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaserMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API ULaserMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULaserMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Movement(float);
	UPROPERTY(EditAnywhere)
	FVector Direction;

	void SetDirection(FVector);
	
	UPROPERTY(EditAnywhere)
	float Speed;

	void SetSpeed(float);

	// How fast the client interpolates to the target location.
	UPROPERTY(EditAnywhere, Category="Laser Settings|Networking")
	float InterpSpeed = 15.0f;

private:

	UPROPERTY(VisibleAnywhere)
	FVector StartPoint;

	// TargetLocation is calculated Server side. Used by clients to interpolate to such location
	UPROPERTY(ReplicatedUsing=OnRep_TargetLocation)
	FVector TargetLocation;
	
	UPROPERTY(VisibleAnywhere, Category="Laser Settings|Networking")
	FVector ClientInterpolationTarget;

	// Called on Clients when TargetLocation is updated.
	UFUNCTION()
	void OnRep_TargetLocation();

	// Required for variable replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
