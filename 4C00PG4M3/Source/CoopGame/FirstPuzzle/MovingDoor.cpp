// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingDoor.h"

#include "CoopGame/Core/CoopGameState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
// Sets default values
AMovingDoor::AMovingDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component and set it as root coponent
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = DoorMesh;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComponent->SetupAttachment(RootComponent);

	// Initializate the status variables of the door. The initial staus of the door is closed and locked
	bIsOpen = false;
}

// Called when the game starts or when spawned
void AMovingDoor::BeginPlay()
{
	Super::BeginPlay();
	if (ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState()))
	{
		GameStateRef->OnAlarmChanged.AddDynamic(this, &AMovingDoor::HandleAlarmChanged);
	}
	ClosedLocation = DoorMesh->GetComponentLocation();
	//Convert local space vector to world space vector so that the door can move correctly in the world
	if (FixedOffsetMovement)
	{
		OpenLocation = ClosedLocation + GetActorTransform().TransformVector(SlideMovementVector);
	}
	else
	{
		
		float DoorLength = DoorMesh->Bounds.BoxExtent.Y * 2.0f;
		float DoorHeight = DoorMesh->Bounds.BoxExtent.Z * 2.0f;
		float DoorWidth = DoorMesh->Bounds.BoxExtent.X * 2.0f;
		
		FVector LocalOffset = FVector::ZeroVector;

		switch (OpenAxis)
		{
		case EDoorOpenAxis::X:
			LocalOffset = FVector(DoorLength, 0.f, 0.f);
			break;
		case EDoorOpenAxis::Y:
			LocalOffset = FVector(0.f, DoorWidth, 0.f);
			break;
		case EDoorOpenAxis::Z:
			LocalOffset = FVector(0.f, 0.f, DoorHeight);
			break;
		}

		if (ReverseMovement)
		{
			LocalOffset *= -1.f;
		}

		// Move along **local axes** relative to the door’s orientation
		OpenLocation = ClosedLocation + DoorMesh->GetComponentTransform().GetUnitAxis(EAxis::X) * LocalOffset.X
									 + DoorMesh->GetComponentTransform().GetUnitAxis(EAxis::Y) * LocalOffset.Y
									 + DoorMesh->GetComponentTransform().GetUnitAxis(EAxis::Z) * LocalOffset.Z;

	}


	if (SlideCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleProgress"));
		DoorTimeline.AddInterpFloat(SlideCurve, ProgressFunction);

		// Apply speed multiplier
		DoorTimeline.SetPlayRate(DoorOpenSpeed);
	}


	if ((DoorType == EDoorType::Gate) && HasAuthority())
	{
		OpenDoor();
	}

	//Binding timeline function
	OnTimelineFinished.BindUFunction(this,FName("PlayEndTimelineSound") );
	
	DoorTimeline.SetTimelineFinishedFunc(OnTimelineFinished);
	
}

// Called every frame
void AMovingDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update only if timeline if DoorTimeline  is active
	DoorTimeline.TickTimeline(DeltaTime);
}


// Open door function
void AMovingDoor::OpenDoor()
{
	//UE_LOG(LogTemp, Warning, TEXT("OPEN DOOR FUNCTION CALL"));
	if (!HasAuthority() || bIsOpen)
	{
		return;
	}
	bIsOpen = true;
	HandleDoorToggle();
}

// Close door function
void AMovingDoor::CloseDoor()
{
	if (!HasAuthority() || !bIsOpen)
	{
		return;
	}
	bIsOpen = false;
	HandleDoorToggle();
}

// Check if the door is open
bool AMovingDoor::IsOpen()
{
	return bIsOpen;
}

void AMovingDoor::HandleProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(ClosedLocation, OpenLocation, Value);
	DoorMesh->SetWorldLocation(NewLocation);
}

void AMovingDoor::HandleAlarmChanged(bool bNewState)
{
	if (DoorType == EDoorType::Gate && HasAuthority())
	{
		if (bNewState)
		{
			CloseDoor();
		}
		else
		{
			OpenDoor();
		}
		
	}
}
void AMovingDoor::OnRep_IsOpen()
{
	HandleDoorToggle();
}


// Replicated variables
void AMovingDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMovingDoor, bIsOpen);
}


void AMovingDoor::HandleDoorToggle()
{
	PlayBeginTimelineSound();
	if (bIsOpen)
	{
		DoorTimeline.PlayFromStart();
	}
	else
	{
		DoorTimeline.Reverse();
	}
}

//Setting sound on Timeline end based on door type
void AMovingDoor::PlayEndTimelineSound() const
{
	if (AudioComponent && DoorType == EDoorType::Gate)
	{
		bIsOpen?AudioComponent->SetSound(OpenDoorSound) : AudioComponent->SetSound(CloseDoorSound);
		AudioComponent->Play();
	}
}
//Setting sound on Timeline start based on door type
void AMovingDoor::PlayBeginTimelineSound() const
{

	if (AudioComponent && (DoorType == EDoorType::Door || DoorType == EDoorType::DoubleSlide))
	{
		bIsOpen?AudioComponent->SetSound(OpenDoorSound) : AudioComponent->SetSound(CloseDoorSound);
		AudioComponent->Play();
	}

}