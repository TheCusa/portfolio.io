// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorLight.h"

#include "Components/SpotLightComponent.h"
#include "CoopGame/Core/CoopGameState.h"


void AFloorLight::HandleAlarmChanged(const bool bNewState)
{
	if (SpotLight)
	{
		SpotLight->SetVisibility(!bNewState);
	}
}

// Sets default values
AFloorLight::AFloorLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));;
	SetRootComponent(StaticMesh);
	SpotLight->SetupAttachment(StaticMesh);
}

// Called when the game starts or when spawned
void AFloorLight::BeginPlay()
{
	Super::BeginPlay();
	if (ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState()))
	{
		GameStateRef->OnAlarmChanged.AddDynamic(this, &AFloorLight::HandleAlarmChanged);
	}
}

