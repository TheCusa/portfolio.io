// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopPlayerState.h"
#include "CoopGame/Core/CoopGameState.h"
#include "CoopGame/Characters/CharacterDefinition.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void ACoopPlayerState::Server_IssueCharacterPicked_Implementation(const UCharacterDefinition* SelectedCharacter)
{

	UE_LOG(LogTemp, Warning, TEXT("ACoopPlayerState::Server_IssueCharacterPicked - Received request from player: %s"), *GetPlayerName());
	
	if (SelectedCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopPlayerState::Server_IssueCharacterPicked - Character to select/deselect: %s"), *SelectedCharacter->CharacterName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopPlayerState::Server_IssueCharacterPicked - Character to deselect (nullptr)."));
	}

	ACoopGameState* CoopGameState = GetWorld()->GetGameState<ACoopGameState>();
	if (!CoopGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("ACoopPlayerState::Server_IssueCharacterPicked - GameState is NOT valid on the server!"));
		return;
	}
	
	// Allow character selection during the correct phase
	if (CoopGameState->GetCurrentLobbyPhase() != ELobbyPhase::CharacterSelection)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to pick character outside of CharacterSelection phase."));
		return;
	}
	
	if (!bIsReady)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player tried to pick a character while not ready."));
		return;
	}
	
	if (!CoopGameState || (!SelectedCharacter && !CurrentSelectedCharacter))
	{
		return;
	}

	if (SelectedCharacter)
	{
		// If we are selecting the same character we already have, deselect it
		if (CurrentSelectedCharacter == SelectedCharacter)
		{
			const UCharacterDefinition* PreviousCharacter = CurrentSelectedCharacter;
			CurrentSelectedCharacter = nullptr;
			CoopGameState->UpdateCharacterSelection(nullptr, PreviousCharacter, this);
		}
		else
		{
			// Check if the character is already selected by another player
			if (CoopGameState->IsCharacterSelected(SelectedCharacter))
			{
				UE_LOG(LogTemp, Warning, TEXT("Character already selected by another player"));
				return;
			}

			// Select the new character
			const UCharacterDefinition* PreviousCharacter = CurrentSelectedCharacter;
			CurrentSelectedCharacter = SelectedCharacter;
			CoopGameState->UpdateCharacterSelection(SelectedCharacter, PreviousCharacter, this);
		}
	}
	// If we are deselecting by passing nullptr
	else if (CurrentSelectedCharacter)
	{
		const UCharacterDefinition* PreviousCharacter = CurrentSelectedCharacter;
		CurrentSelectedCharacter = nullptr;
		CoopGameState->UpdateCharacterSelection(nullptr, PreviousCharacter, this);
	}
}

UClass* ACoopPlayerState::GetPickedCharacterClass() const
{
	if (CurrentSelectedCharacter && CurrentSelectedCharacter->CharacterClass)
	{
		return CurrentSelectedCharacter->CharacterClass;
	}
	return nullptr;
}

void ACoopPlayerState::SetCurrentSelectedCharacter(const UCharacterDefinition* Character)
{
	CurrentSelectedCharacter = Character;
}

void ACoopPlayerState::Server_SetIsReady_Implementation(bool bNewReadyState)
{
	if (bIsReady == bNewReadyState) return;

	bIsReady = bNewReadyState;
	OnRep_IsReady();

	// If the player becomes "Not Ready", deselect their character
	if (!bIsReady && CurrentSelectedCharacter)
	{
		Server_IssueCharacterPicked_Implementation(nullptr);
	}

	// Tell the GameState to check if the phase should change
	if (ACoopGameState* CoopGameState = GetWorld()->GetGameState<ACoopGameState>())
	{
		CoopGameState->CheckAndAdvanceLobbyPhase();
	}
}

void ACoopPlayerState::OnRep_IsReady()
{
	// Broadcast the delegate so the UI can update
	OnPlayerReadyStateChanged.Broadcast(bIsReady);
}

void ACoopPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACoopPlayerState, CurrentSelectedCharacter);
	DOREPLIFETIME(ACoopPlayerState, bIsReady);
}

void ACoopPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (ACoopPlayerState* CoopPS = Cast<ACoopPlayerState>(PlayerState))
	{
		CoopPS->CurrentSelectedCharacter = CurrentSelectedCharacter;
		// Also copy the ready state, although it's likely players will need to ready up again in a new lobby
		CoopPS->bIsReady = false;

		UE_LOG(LogTemp, Warning, TEXT("CopyProperties: Copied character selection '%s' to new PlayerState"),
			CurrentSelectedCharacter ? *CurrentSelectedCharacter->CharacterName.ToString() : TEXT("None"));
	}
}