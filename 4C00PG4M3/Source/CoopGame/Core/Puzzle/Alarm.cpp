// Fill out your copyright notice in the Description page of Project Settings.


#include "Alarm.h"

#include "Components/RectLightComponent.h"
#include "CoopGame/Core/CoopGameState.h"
#include "Components/AudioComponent.h"
// Sets default values
AAlarm::AAlarm()
{
	PrimaryActorTick.bCanEverTick = false;
	RectLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLightComponent"));
	RootComponent = RectLight;
	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	Audio->SetupAttachment(RootComponent);
}

void AAlarm::BeginPlay()
{
	Super::BeginPlay();
	if (ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState()))
	{
		GameStateRef->OnAlarmChanged.AddDynamic(this, &AAlarm::HandleAlarmChanged);
	}

	if (this->ActorHasTag("Odd"))
	{
		RectLight->SetLightFunctionMaterial(MaterialLightAlarmSin);
	}
	if (this->ActorHasTag("Even"))
	{
		RectLight->SetLightFunctionMaterial(MaterialLightAlarmCos);
	}
	
}

// Called every frame
void AAlarm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AAlarm::HandleAlarmChanged(bool bNewState)
{
	if (RectLight)
	{
		//GEngine-> AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Alarm state changed: %s"), bNewState ? TEXT("ON") : TEXT("OFF")));
		RectLight->SetVisibility(bNewState);
	}
	if (bNewState && !Audio->IsPlaying())
	{
		Audio->Play();
	}
	else if (!bNewState && Audio->IsPlaying())
	{
		Audio->Stop();
	}
	
		
		
}

