// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingMovementComponentInterp.h"
#include "Net/UnrealNetwork.h"

URotatingMovementComponentInterp::URotatingMovementComponentInterp()
{
	SetIsReplicatedByDefault(true);
	bRotationInLocalSpace = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void URotatingMovementComponentInterp::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (Owner)
	{
		// Initialize targets to the starting rotation
		ClientInterpolationTarget = Owner->GetActorRotation();
		ReplicatedTargetRotation = Owner->GetActorRotation();
	}
}

void URotatingMovementComponentInterp::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	if (Owner->HasAuthority())
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		ReplicatedTargetRotation = Owner->GetActorRotation();
	}
	else
	{
		if (!ShouldSkipUpdate(DeltaTime))
		{
			FRotator CurrentRot = Owner->GetActorRotation();
			FRotator SmoothedRot = FMath::RInterpTo(CurrentRot, ClientInterpolationTarget, DeltaTime, InterpSpeed);
			Owner->SetActorRotation(SmoothedRot);
		}
	}
}

void URotatingMovementComponentInterp::OnRep_TargetRotation()
{
	ClientInterpolationTarget = ReplicatedTargetRotation;
}

void URotatingMovementComponentInterp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate ReplicatedTargetRotation from Server to Clients
	DOREPLIFETIME_CONDITION(URotatingMovementComponentInterp, ReplicatedTargetRotation, COND_SimulatedOnly);
}
