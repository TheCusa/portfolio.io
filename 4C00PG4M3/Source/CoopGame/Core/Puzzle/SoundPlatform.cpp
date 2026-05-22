// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundPlatform.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CoopGame/SoundPuzzle/Piano.h"
#include "CoopGame/Characters/Agent/AgentCharacter.h"
#include "CoopGame/Core/PlayerControllers/AgentPlayerController.h"
#include "CoopGame/Core/CoopGameState.h"
#include "CoopGame/Core/Puzzle/Utils.h"
#include "Components/BoxComponent.h"


// Sets default values
ASoundPlatform::ASoundPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	if (BoxCollision)
	{
		BoxCollision->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void ASoundPlatform::BeginPlay()
{
	Super::BeginPlay();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASoundPlatform::OnBeginOverlap);
	PianoRef = Cast<APiano>(this->GetParentActor());
}

// Called every frame
void ASoundPlatform::OnBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor->IsA(AAgentCharacter::StaticClass()) && PianoRef && OtherComp->IsA(UBoxComponent::StaticClass()))
	{
		AAgentCharacter* AgentCharacter = Cast<AAgentCharacter>(OtherActor);
		PCRef = Cast<AAgentPlayerController>(AgentCharacter->GetController());

		if (PCRef && !PianoRef->bIsPuzzleSolved)
		{
			PianoRef->AddNote(NoteName); //Populating Array		

			TArray<int8> ConvertedArray = Utils::ConvertArrayToInt8(PianoRef->NoteArray);
			PCRef->SendArrayCode(ConvertedArray, GameUserWidget::SoundPuzzleWidget);
		}
			Multicast_PlaySound();
	}
}
void ASoundPlatform::Multicast_PlaySound_Implementation()
{
	PlayKeySound(NoteName);
}
void ASoundPlatform::PlayKeySound(Notes NoteID)
{
	if (SoundAsset)
	{
		Note = SoundAsset->GetKeySound(NoteID); //Retrieve associated sound from ID
		UGameplayStatics::PlaySound2D(this, Note);
	}
}
