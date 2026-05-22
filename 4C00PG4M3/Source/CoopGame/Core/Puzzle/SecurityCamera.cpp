// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"
#include "Components/SceneCaptureComponent2D.h"
#include "CoopGame/Core/RenderTargetPoolManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASecurityCamera::ASecurityCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Create and setup Static Mesh Component
	CameraStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = CameraStaticMeshComponent;

	CameraCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CameraCaptureComponent"));
	CameraCaptureComponent->SetupAttachment(RootComponent);

	CameraCaptureComponent->bCaptureEveryFrame = false;
	CameraCaptureComponent->bCaptureOnMovement = false;
}

// Called when the game starts or when spawned
void ASecurityCamera::BeginPlay()
{
	Super::BeginPlay();
	
	if (CameraCaptureComponent && IsValid(RenderTargetPoolManager))
	{
		UTextureRenderTarget2D* AssignedTarget = Cast<ARenderTargetPoolManager>(UGameplayStatics::GetActorOfClass(this, ARenderTargetPoolManager::StaticClass()))->GetRenderTarget(SortingOrder); //RenderTargetPoolManager->GetDefaultObject<ARenderTargetPoolManager>()->GetRenderTarget(SortingOrder);
		if (AssignedTarget)
		{
			CameraCaptureComponent->TextureTarget = AssignedTarget;
			UE_LOG(LogTemp, Warning, TEXT("Assigned preallocated RenderTarget to %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No RenderTarget available for %s"), *GetName());
		}
	}
	SetActive(false);
}

void ASecurityCamera::SetActive(bool bIsActive)
{
	if (CameraCaptureComponent)
	{
		CameraCaptureComponent->bCaptureEveryFrame = bIsActive;
	}
}


