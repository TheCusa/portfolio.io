// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "CoopGame/Core/CoopGameState.h"
#include "CoopGame/Core/CoopPlayerState.h"
#include "CoopGame/EOS/EOSGameInstance.h"
#include "Components/TileView.h"
#include "CoopGame/Characters/CharacterDefinition.h"
#include "CharacterEntry.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "CoopGame/Core/PlayerControllers/CoopPlayerController.h"
#include "Engine/World.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GameInstance = GetGameInstance<UEOSGameInstance>();
	PlayerState = GetOwningPlayerState<ACoopPlayerState>();
	GameState = GetWorld()->GetGameState<ACoopGameState>();

	if (ReadyButton)
	{
		ReadyButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnReadyClicked);
	}
	
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::LoadGame);
	}
	
	if (InviteButton)
	{
		InviteButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnInviteClicked);
	}
	
	if (LeaveButton)
	{
		LeaveButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnLeaveClicked);
	}

	if (PlayerState)
	{
		PlayerState->OnPlayerReadyStateChanged.AddDynamic(this, &ULobbyWidget::OnLocalPlayerReadyStateChanged);
		OnLocalPlayerReadyStateChanged(PlayerState->IsReady());
	}

	if (GameState)
	{
	
		GameState->OnLobbyPhaseChanged.AddDynamic(this, &ULobbyWidget::OnLobbyPhaseChanged);
		GameState->OnSessionNameReplicated.AddDynamic(this, &ULobbyWidget::SessionNameReplicated);
		GameState->OnPlayerSelectionsChanged.AddDynamic(this, &ULobbyWidget::UpdateAllCharacterEntries);
		
		if (CharacterList)
		{
			const TArray<UCharacterDefinition*>& CharacterDefs = GameState->GetCharacters();
			if (CharacterDefs.Num() > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("NativeConstruct: Populating CharacterList with %d items."), CharacterDefs.Num());
				CharacterList->SetListItems(CharacterDefs);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NativeConstruct: GameState has no characters to show."));
			}
		}
		
		if (PlayerList)
		{
			RefreshPlayerList();
		}
		
		OnLobbyPhaseChanged(GameState->GetCurrentLobbyPhase());
		SessionNameReplicated(GameState->GetSessionName());

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NativeConstruct: GameState is NULLPTR! Lobby UI will not function correctly."));
	}
	
	if (CharacterList)
	{
		CharacterList->OnItemClicked().AddUObject(this, &ULobbyWidget::PlayerSelectionIssued);
	}

	// Timers
	GetWorld()->GetTimerManager().SetTimer(PlayerListUpdateTimerHandle, this, &ULobbyWidget::RefreshPlayerList, 0.5f, true);
	GetWorld()->GetTimerManager().SetTimer(StartButtonUpdateTimerHandle, this, &ULobbyWidget::UpdateStartButtonState, 0.5f, true);
	
	// Set Button visibility
	UpdateStartButtonVisibility();
	if (InviteButton)
	{
		InviteButton->SetVisibility(IsHost() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void ULobbyWidget::OnInviteClicked()
{
	if (GameInstance)
	{
		GameInstance->ShowFriendsUI();
	}
}

void ULobbyWidget::OnReadyClicked()
{
	if (PlayerState)
	{
		// Call the server RPC to toggle the ready state
		PlayerState->Server_SetIsReady(!PlayerState->IsReady());
	}
}

void ULobbyWidget::OnLeaveClicked()
{
	ACoopPlayerController* PC = GetOwningPlayer<ACoopPlayerController>();
	if (PC)
	{
		if (IsHost())
		{
			if (GameInstance)
			{
				GameInstance->ReturnToMainMenu();
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Client requesting to leave session via RPC."));
			PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Leaving session.")));
		}
	}
}

void ULobbyWidget::OnLobbyPhaseChanged(ELobbyPhase NewPhase)
{
	if (!LobbyStateSwitcher) return;

	switch (NewPhase)
	{
	case ELobbyPhase::ReadyUp:
		LobbyStateSwitcher->SetActiveWidgetIndex(0);
		break;
	case ELobbyPhase::CharacterSelection:
		LobbyStateSwitcher->SetActiveWidgetIndex(1);
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TempHandle, 
			this, 
			&ULobbyWidget::UpdateAllCharacterEntries, 
			0.1f,
			false
		);
		break;
	}
}

void ULobbyWidget::OnLocalPlayerReadyStateChanged(const bool bIsReady)
{
	if (ReadyButtonText)
	{
		ReadyButtonText->SetText(bIsReady ? FText::FromString(TEXT("NOT READY")) : FText::FromString(TEXT("READY")));
	}
	if (ReadyButton)
	{
		ReadyButton->SetBackgroundColor(bIsReady ? ButtonNotReadyColor : ButtonReadyColor);
	}
}

void ULobbyWidget::UpdateAllCharacterEntries()
{
	if (!GameState || !CharacterList) return;

	ENetMode NetMode = GetWorld()->GetNetMode();
	FString RoleString = (NetMode == NM_ListenServer || NetMode == NM_DedicatedServer) ? TEXT("Server/Host") : FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID() > 0 ? UE::GetPlayInEditorID() - 1 : 0);
	UE_LOG(LogTemp, Warning, TEXT("--- [%s] Executing UpdateAllCharacterEntries ---"), *RoleString);
	
	const int32 NumItems = CharacterList->GetNumItems();
	for (int32 i = 0; i < NumItems; ++i)
	{
		UCharacterDefinition* CharacterDef = Cast<UCharacterDefinition>(CharacterList->GetItemAt(i));
		if (!CharacterDef) continue;
		
		UCharacterEntry* Entry = CharacterList->GetEntryWidgetFromItem<UCharacterEntry>(CharacterDef);
		
		if (Entry)
		{
			APlayerState* OwningPlayer = GameState->GetPlayerStateForSelectedCharacter(CharacterDef);
			
			if (OwningPlayer)
			{
				UE_LOG(LogTemp, Log, TEXT("[%s] Updating Entry for '%s': Owner is '%s'"), *RoleString, *CharacterDef->CharacterName.ToString(), *OwningPlayer->GetPlayerName());
				Entry->SetCharacterSelected(true);
				Entry->SetOwningPlayerName(OwningPlayer->GetPlayerName());
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[%s] Updating Entry for '%s': No owner."), *RoleString, *CharacterDef->CharacterName.ToString());
				Entry->SetCharacterSelected(false);
				Entry->SetOwningPlayerName(FString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Could not find entry widget for character '%s' at index %d"), *RoleString, *CharacterDef->CharacterName.ToString(), i);
		}
	}
}

void ULobbyWidget::NativeDestruct()
{
	// Clean timer for avoid crash
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerListUpdateTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(StartButtonUpdateTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(InitialSyncTimerHandle);
	}

	Super::NativeDestruct();
}

void ULobbyWidget::SessionNameReplicated(const FName& NewSessionName)
{
	LobbyNameText->SetText(FText::FromName(NewSessionName));
}

bool SortPlayerStates(const APlayerState& A, const APlayerState& B)
{
	// If A = server, B = client, A first
	if (A.GetOwner() && A.GetOwner()->HasAuthority() && (!B.GetOwner() || !B.GetOwner()->HasAuthority()))
	{
		return true;
	}
	// If B = server, A = client, B first
	if (B.GetOwner() && B.GetOwner()->HasAuthority() && (!A.GetOwner() || !A.GetOwner()->HasAuthority()))
	{
		return false;
	}
	return A.GetPlayerId() < B.GetPlayerId();
}

void ULobbyWidget::RefreshPlayerList()
{
	if (GameState && PlayerList) 
	{
		TArray<APlayerState*> PlayerStates = GameState->PlayerArray;
		PlayerStates.Sort([](const APlayerState& A, const APlayerState& B)
				{
					const bool bAIsHost = A.GetOwner() && A.GetOwner()->HasAuthority();
					const bool bBIsHost = B.GetOwner() && B.GetOwner()->HasAuthority();

					if (bAIsHost != bBIsHost)
					{
						return bAIsHost;
					}
					return A.GetPlayerId() < B.GetPlayerId();
				});
		
		PlayerList->SetListItems(PlayerStates);
		UpdateStartButtonState();
	}
}

void ULobbyWidget::PlayerSelectionIssued(UObject* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("ULobbyWidget::PlayerSelectionIssued - Click detected on an item."));

	UCharacterDefinition* SelectedCharacterDefinition = Cast<UCharacterDefinition>(Item);
	
	if (!SelectedCharacterDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("ULobbyWidget::PlayerSelectionIssued - Clicked item could not be cast to UCharacterDefinition."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ULobbyWidget::PlayerSelectionIssued - Selected character is: %s"), *SelectedCharacterDefinition->CharacterName.ToString());
	
	if (PlayerState)
	{
		PlayerState->Server_IssueCharacterPicked(SelectedCharacterDefinition);
		if (CharacterList)
		{
			CharacterList->SetSelectedItem(nullptr);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ULobbyWidget::PlayerSelectionIssued - PlayerState is NOT valid! Cannot send selection to server."));
	}
}

void ULobbyWidget::LoadGame()
{
	if (!IsHost())
	{
		UE_LOG(LogTemp, Warning, TEXT("Only host can start the game"));
		return;
	}
	
	if (!GameState || !GameState->CanStartGame())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot start game: not all players have selected characters"));
		return;
	}
	
	if (GameInstance)
	{
		GameInstance->LoadGameLevel();
	}
}

bool ULobbyWidget::IsHost() const
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		return PC->HasAuthority();
	}
	return false;
}

void ULobbyWidget::UpdateStartButtonVisibility()
{
	if (StartButton)
	{
		StartButton->SetVisibility(IsHost() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void ULobbyWidget::UpdateStartButtonState()
{
	if (StartButton && IsHost())
	{
		bool bCanStart = GameState ? GameState->CanStartGame() : false;
		StartButton->SetIsEnabled(bCanStart);
		
		if (bCanStart)
		{
			StartButton->SetToolTipText(FText::FromString(TEXT("Start Game")));
		}
		else
		{
			int32 PlayersReady = GameState ? GameState->GetSelectedCharactersCount() : 0;
			FString TooltipText = FString::Printf(TEXT("Players ready: %d/2"), PlayersReady);
			StartButton->SetToolTipText(FText::FromString(TooltipText));
		}
	}
}