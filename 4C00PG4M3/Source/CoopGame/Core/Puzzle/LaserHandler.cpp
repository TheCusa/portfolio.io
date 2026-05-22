// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserHandler.h"
#include "Components/BoxComponent.h"
#include "CoopGame/Characters/Agent/AgentCharacter.h"
#include "CoopGame/Core/CoopGameState.h"
#include "CoopGame/Core/Puzzle/LaserEndPoint.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALaserHandler::ALaserHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>("LaserMesh");
	RootComponent = Collider;
	LaserMesh->SetupAttachment(Collider);
	bReplicates = true;
    AActor::SetReplicateMovement(true); // ReplicateMovement handled manually from the LaserMovementComponent
}

// Called when the game starts or when spawned
void ALaserHandler::BeginPlay()
{
	Super::BeginPlay();
	//RespawnPointRef = Cast<ARespawnPoint>(UGameplayStatics::GetActorOfClass(this, ARespawnPoint::StaticClass()));
	if (Collider)
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &ALaserHandler::OnBeginOverlap);
	}
}

void ALaserHandler::OnBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	//Checking if the instance has authority, otherwise an RPC gets called
		
	if (HasAuthority())
	{
		HandleCollision(OtherActor, OtherComp);
	}
}

void ALaserHandler::HandleCollision(AActor* ActorHit, UPrimitiveComponent* ComponentHit)
{
	if (ActorHit)
	{
		if (ActorHit->IsA(AAgentCharacter::StaticClass()) && bIsReal && HasAuthority() && ComponentHit->IsA(UCapsuleComponent::StaticClass()))
		{
			//ActorHit->SetActorLocation(RespawnPointRef->RespawnPosition, false);
#if !UE_BUILD_SHIPPING
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Hit");
#endif
			ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState());
			GameStateRef->SetAlarm(true);
		}
		else if (ActorHit->IsA(ALaserEndPoint::StaticClass()))
		{
			Destroy();
		}
	}
}

