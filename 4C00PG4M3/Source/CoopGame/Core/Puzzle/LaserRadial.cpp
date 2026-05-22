// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGame/Core/Puzzle/LaserRadial.h"
#include "Components/BoxComponent.h"

 ALaserRadial::ALaserRadial()
 {
 	ColliderHorizontal = CreateDefaultSubobject<UBoxComponent>(TEXT("ColliderHorizontal"));
 	HorizontalLaserMesh = CreateDefaultSubobject<UStaticMeshComponent>("LaserHorizontalMesh");
 	HorizontalLaserMesh->SetupAttachment(ColliderHorizontal);
 	ColliderHorizontal->SetupAttachment(GetRootComponent());
 	AActor::SetReplicateMovement(false); // ReplicateMovement handled manually from the LaserMovementComponent
 }
void ALaserRadial::BeginPlay()
{
  Super::BeginPlay();
 	if (ColliderHorizontal)
 	{
 		ColliderHorizontal->OnComponentBeginOverlap.AddDynamic(this, &ALaserRadial::OnBeginOverlap);
 	}
}
void ALaserRadial::OnBeginOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
 {
	 Super::OnBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
 }



