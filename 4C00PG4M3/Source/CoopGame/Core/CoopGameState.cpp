// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopGameState.h"
#include "Net/UnrealNetwork.h"
#include "CoopGame/Characters/CharacterDefinition.h"
#include "CoopGame/Core/CoopPlayerState.h"
#include "GameFramework/PlayerState.h"

void ACoopGameState::SetSessionName(const FName& NewSessionName)
{
	SessionName = NewSessionName;
}

void ACoopGameState::CheckAndAdvanceLobbyPhase()
{
	if (!HasAuthority())
	{
		return;
	}
	
	// If a player leaves, cancel any transition and revert to the ReadyUp phase.
	if (PlayerArray.Num() < MaxPlayers)
	{
		CancelPhaseTransition();
		if (CurrentLobbyPhase != ELobbyPhase::ReadyUp)
		{
			CurrentLobbyPhase = ELobbyPhase::ReadyUp;
			OnRep_LobbyPhase();
		}
		return;
	}
	
	// Check if all connected players are marked as ready.
	bool bAllPlayersReady = true;
	for (APlayerState* PS : PlayerArray)
	{
		const ACoopPlayerState* CoopPS = Cast<ACoopPlayerState>(PS);
		if (!CoopPS || !CoopPS->IsReady())
		{
			bAllPlayersReady = false;
			break;
		}
	}

	if (bAllPlayersReady)
	{
		if (CurrentLobbyPhase == ELobbyPhase::ReadyUp && !GetWorld()->GetTimerManager().IsTimerActive(PhaseTransitionTimerHandle))
		{
			// If everyone is ready, and we are not already transitioning, start the timer.
			UE_LOG(LogTemp, Log, TEXT("All players are ready. Starting 2-second timer to switch to character selection."));
			GetWorld()->GetTimerManager().SetTimer(
				PhaseTransitionTimerHandle, 
				this, 
				&ACoopGameState::StartCharacterSelectionPhase, 
				2.0f, 
				false
			);
		}
	}
	else
	{
		// If a player becomes un-ready, cancel the transition and revert to the ReadyUp phase.
		CancelPhaseTransition();
		if (CurrentLobbyPhase == ELobbyPhase::CharacterSelection)
		{
			UE_LOG(LogTemp, Log, TEXT("A player is no longer ready. Returning to ReadyUp phase."));
			CurrentLobbyPhase = ELobbyPhase::ReadyUp;
			OnRep_LobbyPhase();
		}
	}
	
}

bool ACoopGameState::CanStartGame() const
{
	if (CurrentLobbyPhase != ELobbyPhase::CharacterSelection)
	{
		return false;
	}
	
	// Check if there are enough players and if everyone has selected a character.
	return PlayerArray.Num() >= MaxPlayers && PlayerSelections.Num() >= MaxPlayers;
}

const TArray<TObjectPtr<UCharacterDefinition>>& ACoopGameState::GetCharacters() const
{
	return Characters;
}

bool ACoopGameState::IsCharacterSelected(const UCharacterDefinition* CharacterToCheck) const
{
	return PlayerSelections.ContainsByPredicate([CharacterToCheck](const FPlayerCharacterSelection& Selection)
	{
		return Selection.Character == CharacterToCheck;
	});
}

void ACoopGameState::UpdateCharacterSelection(const UCharacterDefinition* Selected, const UCharacterDefinition* Deselected, APlayerState* Player)
{
	if (!HasAuthority()) return;

	PlayerSelections.RemoveAll([Player](const FPlayerCharacterSelection& Selection)
		{
			return Selection.Player == Player;
		});
	
	// If a character was deselected, ensure no one else has it.
	if (Deselected)
	{
		PlayerSelections.RemoveAll([Deselected](const FPlayerCharacterSelection& Selection)
		{
			return Selection.Character == Deselected;
		});
	}
	
	// If a new character is being selected, add it to the array.
	if (Selected)
	{
		FPlayerCharacterSelection NewSelection;
		NewSelection.Player = Player;
		NewSelection.Character = Selected;
		PlayerSelections.Add(NewSelection);
	}

	OnRep_PlayerSelections();
}

const UCharacterDefinition* ACoopGameState::GetCharacterSelectedByPlayer(APlayerState* Player) const
{
	if (!Player)
	{
		return nullptr;
	}
	
	// Finds the entry in the array corresponding to the given player.
	const FPlayerCharacterSelection* FoundSelection = PlayerSelections.FindByPredicate([Player](const FPlayerCharacterSelection& Selection)
	{
		return Selection.Player == Player;
	});
	return FoundSelection ? FoundSelection->Character.Get() : nullptr;
}

TObjectPtr<APlayerState> ACoopGameState::GetPlayerStateForSelectedCharacter(const UCharacterDefinition* Character) const
{
	if (!Character)
	{
		return nullptr;
	}
	const FPlayerCharacterSelection* FoundSelection = PlayerSelections.FindByPredicate([Character](const FPlayerCharacterSelection& Selection)
	{
		return Selection.Character == Character;
	});

	return FoundSelection ? FoundSelection->Player.Get() : nullptr;
}

void ACoopGameState::SetAlarm(const bool bValue)
{
	bIsAlarmActive = bValue;
	OnAlarmChanged.Broadcast(bIsAlarmActive);
	//UE_LOG( LogTemp, Warning, TEXT("Alarm State changed to: %s"), bIsAlarmActive ? TEXT("ACTIVE") : TEXT("INACTIVE"));
}

void ACoopGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// List of all properties that need to be synchronized from server to clients.
	DOREPLIFETIME(ACoopGameState, SessionName);
	DOREPLIFETIME(ACoopGameState, CodePuzzleSolution);
	DOREPLIFETIME(ACoopGameState, HackerPlayerController);
	DOREPLIFETIME(ACoopGameState, AgentPlayerController);
	DOREPLIFETIME(ACoopGameState, bIsAlarmActive);
	DOREPLIFETIME(ACoopGameState, GameTimer);
	DOREPLIFETIME(ACoopGameState, CurrentLobbyPhase);
	DOREPLIFETIME(ACoopGameState, PlayerSelections);
}

void ACoopGameState::OnRep_SessionName()
{
	OnSessionNameReplicated.Broadcast(SessionName);
}

void ACoopGameState::OnRep_LobbyPhase()
{
	OnLobbyPhaseChanged.Broadcast(CurrentLobbyPhase);
}

void ACoopGameState::OnRep_PlayerSelections()
{
	OnPlayerSelectionsChanged.Broadcast();
}

void ACoopGameState::OnRep_IsAlarmActive()
{
	OnAlarmChanged.Broadcast(bIsAlarmActive);
}

void ACoopGameState::StartCharacterSelectionPhase()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("Timer expired. Switching to CharacterSelection phase."));
		CurrentLobbyPhase = ELobbyPhase::CharacterSelection;
		OnRep_LobbyPhase();
	}
}

void ACoopGameState::CancelPhaseTransition()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(PhaseTransitionTimerHandle))
	{
		UE_LOG(LogTemp, Log, TEXT("Phase transition cancelled."));
		GetWorld()->GetTimerManager().ClearTimer(PhaseTransitionTimerHandle);
	}
}
