// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerState.h"


void ACoopPlayerController::Say(const FString& Message)
{
	if (Message.IsEmpty())
	{
		return;
	}
	Server_SendMessage(Message);
}

void ACoopPlayerController::Client_GameOver_Implementation()
{
	if (GameOverWidgetClass)
	{
		GameOverWidgetInstance = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		if (GameOverWidgetInstance)
		{
			GameOverWidgetInstance->AddToViewport(100);
		}
	}
	
	FInputModeUIOnly InputMode;
	if (GameOverWidgetInstance)
	{
		InputMode.SetWidgetToFocus(GameOverWidgetInstance->TakeWidget());
	}
	else
	{
		InputMode.SetWidgetToFocus(nullptr);
	}
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	SetPause(true);
}

void ACoopPlayerController::OnRep_PlayerState()
{
	BP_OnRep_PlayerState();
}

void ACoopPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ACoopPlayerController::Server_SendMessage_Implementation(const FString& Message)
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	APlayerState* SenderPlayerState = GetPlayerState<APlayerState>();

	if (!SenderPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_SendMessage: PlayerState is null for a controller. Cannot get sender name."));
		return; 
	}

	const FString SenderName = SenderPlayerState->GetPlayerName();

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		ACoopPlayerController* TargetPC = Cast<ACoopPlayerController>(It->Get());

		if (TargetPC && TargetPC != this)
		{
			TargetPC->Client_ReceiveMessage(Message, SenderName);
		}
	}
}

void ACoopPlayerController::Client_ReceiveMessage_Implementation(const FString& Message, const FString& SenderName)
{
	if (GEngine)
	{
		const FString FinalMessage = FString::Printf(TEXT("[%s] dice: %s"), *SenderName, *Message);
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FinalMessage);
	}
}



void ACoopPlayerController::Multicast_ShowDebugMessage_Implementation(const FString& Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
	}
}

