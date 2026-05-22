// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserGroupRotating.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "CoopGame/Core/Components/RotatingMovementComponentInterp.h"


// Sets default values
ALaserGroupRotating::ALaserGroupRotating()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	RootPivot = CreateDefaultSubobject<USceneComponent>(TEXT("RootPivot"));
	RootComponent = RootPivot;

	RotationComponent = CreateDefaultSubobject<URotatingMovementComponentInterp>(TEXT("RotationComponent"));
	RotationComponent->SetUpdatedComponent(RootPivot);

	// (Editor Only)
	PreviewRealISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PreviewRealISMC"));
	PreviewRealISMC->SetupAttachment(RootPivot);
	PreviewRealISMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewRealISMC->bIsEditorOnly = true;
	PreviewRealISMC->SetHiddenInGame(true);

	PreviewFakeISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PreviewFakeISMC"));
	PreviewFakeISMC->SetupAttachment(RootPivot);
	PreviewFakeISMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewFakeISMC->bIsEditorOnly = true;
	PreviewFakeISMC->SetHiddenInGame(true);
}

void ALaserGroupRotating::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
#if !UE_BUILD_SHIPPING
	if (!PreviewRealISMC || !PreviewFakeISMC)
	{
		return;
	}
    PreviewRealISMC->ClearInstances();
    PreviewFakeISMC->ClearInstances();

    if (LaserPattern.Num() == 0 || !PreviewMesh) return;
	
    PreviewRealISMC->SetStaticMesh(PreviewMesh);
    if (PreviewMaterialReal) PreviewRealISMC->SetMaterial(0, PreviewMaterialReal);

    PreviewFakeISMC->SetStaticMesh(PreviewMesh);
    if (PreviewMaterialFake) PreviewFakeISMC->SetMaterial(0, PreviewMaterialFake);
	
    int32 TotalCount = LaserPattern.Num();
    
    for (int32 i = 0; i < TotalCount; i++)
    {
        FVector Offset = CalculateLaserOffset(i, TotalCount, Radius);
        FTransform InstanceTransform;
        InstanceTransform.SetLocation(Offset);
        if (LaserPattern[i].Type == ELaserType::Real)
        {
            PreviewRealISMC->AddInstance(InstanceTransform);
        }
        else
        {
            PreviewFakeISMC->AddInstance(InstanceTransform);
        }
    }
#endif
}

void ALaserGroupRotating::BeginPlay()
{
    Super::BeginPlay();
	
    if (HasAuthority())
    {
    	int32 TotalCount = LaserPattern.Num();
    	
        for (int32 i = 0; i < TotalCount; i++)
        {
            TSubclassOf<AActor> ClassToSpawn = nullptr;
            if (LaserPattern[i].Type == ELaserType::Real)
            {
                ClassToSpawn = RealLaserClass;
            }
            else
            {
                ClassToSpawn = FakeLaserClass;
            }
        	
            if (ClassToSpawn)
            {
                FVector Offset = CalculateLaserOffset(i, TotalCount, Radius);
            	
                FVector SpawnLoc = GetActorLocation() + GetActorRotation().RotateVector(Offset);
                FRotator SpawnRot = GetActorRotation();

                FActorSpawnParameters Params;
                Params.Owner = this;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                AActor* NewLaser = GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnLoc, SpawnRot, Params);

                if (NewLaser)
                {
                    NewLaser->AttachToComponent(RootPivot, FAttachmentTransformRules::KeepWorldTransform);
                }
            }
        }
    }
}

FVector ALaserGroupRotating::CalculateLaserOffset(int32 Index, int32 TotalCount, float CurrentRadius)
{
    if (TotalCount <= 0) return FVector::ZeroVector;
    float AngleDeg = (360.0f / TotalCount) * Index;
    float AngleRad = FMath::DegreesToRadians(AngleDeg);
    
    return FVector(CurrentRadius * FMath::Cos(AngleRad), CurrentRadius * FMath::Sin(AngleRad), 0.0f);
}