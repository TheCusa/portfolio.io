// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGame/FirstPuzzle/InteractableActor.h"
#include "Components/BoxComponent.h"
#include "CoopGame/Characters/CharacterParentClass.h"


// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the box collision component
	InteractionCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ColliderComponent"));
	InteractionCollider->InitBoxExtent(FVector(30.0f, 30.0f, 30.0f));

	// Set collision as query only for overlaps events only with pawn
	InteractionCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	InteractionCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractionCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Set box collision component as root component
	RootComponent = InteractionCollider;

	// Delegate the overlap functions
	InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::OnInteractionBoxOverlapBegin);
	InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OnInteractionBoxOverlapEnd);
}

// Default implementation
void  AInteractableActor::ExecuteAction()
{
	UE_LOG(LogTemp, Display, TEXT("Super::ExecuteAction Called!"));
}


// Get text for the UI prompt function
FText AInteractableActor::GetUIPromptText() const
{
	return FText::FromString("Press E");
}


// ------ OVERLAPS FUNCTIONS -------
void AInteractableActor::OnInteractionBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("BEGIN OVERLAP"));

	if (!OtherActor)
	{
		return;
	}

	ACharacterParentClass* OverlapCharacter = Cast<ACharacterParentClass>(OtherActor);
	if (OverlapCharacter)
	{
		// Check if OverlapChracter is the local player. It's needed to execute this event only in local
		if (!OverlapCharacter->IsLocallyControlled())
		{
			return;
		}
		OverlapCharacter->SetNearbyInteractableObject(this);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetName());
	}
}

void AInteractableActor::OnInteractionBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("END OVERLAP"));

	if (!OtherActor)
	{
		return;
	}

	ACharacterParentClass* OverlapCharacter = Cast<ACharacterParentClass>(OtherActor);
	if (OverlapCharacter)
	{
		// Check if OverlapChracter is the local player. It's needed to execute this event only in local
		if (!OverlapCharacter->IsLocallyControlled())
		{
			return;
		}

		OverlapCharacter->ClearNearbyInteractableObject(this);
	}
}

