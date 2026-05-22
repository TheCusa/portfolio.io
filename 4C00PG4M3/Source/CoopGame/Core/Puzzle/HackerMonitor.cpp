// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGame/Core/Puzzle/HackerMonitor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "CoopGame/Characters/Hacker/HackerCharacter.h"
#include "CoopGame/Core/PlayerControllers/HackerPlayerController.h"
#include "CoopGame/SoundPuzzle/AudioSequenceComponent.h"


// Sets default values
AHackerMonitor::AHackerMonitor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    // Create and setup Static Mesh Component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    //RootComponent = MeshComponent;
	MeshComponent->SetupAttachment(RootComponent);
    // Create and setup Widget Component
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(RootComponent);

    // Configure widget settings
    WidgetComponent->SetDrawAtDesiredSize(true);
    WidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	// Create Audio Sequence Component
	AudioSequenceComponent = CreateDefaultSubobject<UAudioSequenceComponent>(TEXT("AudioSequenceComponent"));	
	IsActive = false;
}

void AHackerMonitor::TriggerAudioSequenceForClient(APlayerController* TargetClient, const TArray<int8>& Notes)
{
	if (!TargetClient)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid TargetClient"));
		return;
	}

	// RPC
	ClientPlayAudioSequence(Notes);
	UE_LOG(LogTemp, Display, TEXT("Triggering audio sequence for client: %s"), *TargetClient->GetName());
}

void AHackerMonitor::ClientPlayAudioSequence_Implementation(const TArray<int8>& Notes)
{
	if (!AudioSequenceComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AudioSequenceComponent is null on client!"));
		return;
	}
    
	UE_LOG(LogTemp, Display, TEXT("Playing audio sequence on client"));
	AudioSequenceComponent->PlayNoteSequence(Notes);
}

void AHackerMonitor::StopAudioSequence() const
{
	if (AudioSequenceComponent)
	{
		AudioSequenceComponent->StopSequence();
	}
}

void AHackerMonitor::ExecuteAction()
{
	Super::ExecuteAction();
	if (!IsActive)
	{
		LoadInputMode();
	}
	else
	{
		RestoreInputMode();
	}
}

void AHackerMonitor::LoadInputMode()
{
	IsActive = true;
	FInputModeGameAndUI InputMode;
	ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState());
	
	if (AHackerPlayerController* HackerPC = GameStateRef->HackerPlayerController)
	{
		AHackerCharacter* CharRef = Cast<AHackerCharacter>(HackerPC->GetPawn());
		CharRef->LockCharacterMovement();
		HackerPC->SetInputMode(InputMode);
		HackerPC->SetViewTargetWithBlend(this, 0.5f);
		CharRef->GetMesh()->SetVisibility(false);
		HackerPC->SetShowMouseCursor(true);
	}
}

void AHackerMonitor::RestoreInputMode()
{
	IsActive = false;
	FInputModeGameOnly InputMode;
	ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState());

	if (AHackerPlayerController* HackerPC = GameStateRef->HackerPlayerController)
	{
		ACharacterParentClass* CharRef = Cast<ACharacterParentClass>(HackerPC->GetPawn());
		CharRef->UnlockCharacterMovement();
		HackerPC->SetInputMode(InputMode);
		HackerPC->SetViewTargetWithBlend(CharRef, 0.5f);
		CharRef->GetMesh()->SetVisibility(true);
		HackerPC->SetShowMouseCursor(false);
	}
}


