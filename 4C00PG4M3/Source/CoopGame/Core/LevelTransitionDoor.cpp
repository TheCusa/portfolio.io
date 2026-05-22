// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionDoor.h"
#include "LevelStreamingManager.h"
#include "Components/BoxComponent.h"
#include "CoopGame/Characters/Agent/AgentCharacter.h"
#include "Kismet/GameplayStatics.h"

void ALevelTransitionDoor::BeginPlay()
{
	Super::BeginPlay();
	
	FarSideTrigger->SetCollisionProfileName("Trigger");
	FarSideTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FarSideTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	FarSideTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FarSideTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	FarSideTrigger->SetGenerateOverlapEvents(true);
	
}

ALevelTransitionDoor::ALevelTransitionDoor()
{

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	DoorMesh->SetupAttachment(SceneRoot);
	
	FarSideTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FarSideTrigger"));
	FarSideTrigger->SetupAttachment(RootComponent);

	OneWayBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("OneWayBlocker"));
	OneWayBlocker->SetupAttachment(RootComponent);
	OneWayBlocker->SetCollisionProfileName("BlockAll");
	OneWayBlocker->SetVisibility(false);
	OneWayBlocker->SetHiddenInGame(true);
	OneWayBlocker->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FarSideTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionDoor::OnFarSideEntered);
	FarSideTrigger->OnComponentEndOverlap.AddDynamic(this, &ALevelTransitionDoor::OnFarSideExited);

}

void ALevelTransitionDoor::OpenDoor()
{
	Super::OpenDoor();

	if (!HasAuthority()) return;
	if (IsValid(LevelStreamingManager))
	{
		LevelStreamingManager->RequestLoad(LevelToLoad);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LevelStreamingManager is null or invalid"));
	}
	
}

void ALevelTransitionDoor::OnFarSideEntered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (!HasAuthority()) return;

	if (OtherActor && OtherActor->IsA(ACharacterParentClass::StaticClass())){
		OneWayBlocker->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Actor"));
	}
}

void ALevelTransitionDoor::OnFarSideExited(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;

	if (OtherActor && OtherActor->IsA(ACharacterParentClass::StaticClass())){
		{
			CloseDoor();
		}
	}
}

void ALevelTransitionDoor::PlayEndTimelineSound() const
{
	Super::PlayEndTimelineSound();
	if (!bIsOpen)
	{
		LevelStreamingManager->RequestUnload(LevelToUnload);
	}
}

