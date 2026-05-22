// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

class UBoxComponent;
class AAgentPlayerController;

UCLASS()
class COOPGAME_API AInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

	// Function that implement the execution of an action whe something interact with this object. Must be ovveride in the derived class.
	virtual void ExecuteAction();

	// Get text for the UI prompt function
	virtual FText GetUIPromptText() const;

	UFUNCTION()
	virtual void OnInteractionBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnInteractionBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

	// Collider reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider")
	UBoxComponent* InteractionCollider;
	
};
