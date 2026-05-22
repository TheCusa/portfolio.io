// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorOpenerTrigger.h"
#include "Components/BoxComponent.h"
#include "CoopGame/Characters/CharacterParentClass.h"
#include "CoopGame/FirstPuzzle/MovingDoor.h"

// Sets default values
ADoorOpenerTrigger::ADoorOpenerTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("OneWayBlocker"));
	BoxTrigger->SetupAttachment(RootComponent);
	BoxTrigger->SetCollisionProfileName("Trigger");
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BoxTrigger->SetGenerateOverlapEvents(true);
	
	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADoorOpenerTrigger::OnBoxTriggerEntered);
}

// Called when the game starts or when spawned
void ADoorOpenerTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoorOpenerTrigger::OnBoxTriggerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (OtherActor && OtherActor->IsA(ACharacterParentClass::StaticClass())){
		if (TargetDoors.Num() <= 0)
			return;

		for (AMovingDoor* const& TargetDoor : TargetDoors)
		{
			if (TargetDoor)
			{
				if (!TargetDoor->IsOpen())
				{
					TargetDoor->OpenDoor();
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TargetDoor not assigned!"));
			}
		}
	}
}
