// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"


class UWidgetSwitcher;
class UCharacterDefinition;
class UTextBlock;
class UButton;
class UListView;
class UTileView;
class UEOSGameInstance;
class ACoopGameState;
class ACoopPlayerState;

/**
 * 
 */
UCLASS()
class COOPGAME_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> LobbyStateSwitcher;

	// Widgets for Phase 1 (Ready-Up)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UListView> PlayerList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> InviteButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeaveButton;
	
	// Widgets for Phase 2 (Character Selection)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LobbyNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> CharacterList;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY()
	TObjectPtr<ACoopGameState> GameState;

	UPROPERTY()
	TObjectPtr<ACoopPlayerState> PlayerState;

	UPROPERTY()
	TObjectPtr<UEOSGameInstance> GameInstance;

	// Timer handles
	FTimerHandle PlayerListUpdateTimerHandle;
	FTimerHandle StartButtonUpdateTimerHandle;
	FTimerHandle InitialSyncTimerHandle;

	UFUNCTION()
	void SessionNameReplicated(const FName& NewSessionName);
	
	UFUNCTION()
	void LoadGame();
	
	bool IsHost() const;
	void UpdateStartButtonVisibility();
	void UpdateStartButtonState();

	void RefreshPlayerList();
	void PlayerSelectionIssued(UObject* Item);

	UFUNCTION()
	void OnInviteClicked();

	UFUNCTION()
	void OnReadyClicked();
	
	UFUNCTION()
	void OnLeaveClicked();

	UFUNCTION()
	void OnLobbyPhaseChanged(ELobbyPhase NewPhase);

	UFUNCTION()
	void OnLocalPlayerReadyStateChanged(bool bIsReady);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor ButtonReadyColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor ButtonNotReadyColor;

	UFUNCTION()
	void UpdateAllCharacterEntries();
};