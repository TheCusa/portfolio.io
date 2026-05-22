// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CoopGameState.generated.h"

class AAgentPlayerController;
class AHackerPlayerController;
class UCharacterDefinition;
class FLifetimeProperty;

/** Struct to hold the association between a player and their selected character. Used for replication. */
USTRUCT()
struct FPlayerCharacterSelection
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<APlayerState> Player;

	UPROPERTY()
	TObjectPtr<const UCharacterDefinition> Character;
};

/** Defines the current phase of the pre-game lobby. */
UENUM(BlueprintType)
enum class ELobbyPhase : uint8
{
	ReadyUp				UMETA(DisplayName = "Ready Up"),
	CharacterSelection	UMETA(DisplayName = "Character Selection")
};

/** Defines the type of puzzle in the game. */
UENUM(BlueprintType)
enum class EPuzzleType : uint8
{
	None        UMETA(DisplayName="None"),
	RoomTemperatureIQ UMETA(DisplayName="RoomTemperatureIQ Puzzle"),
	LaserRun UMETA(DisplayName="LaserRun Puzzle"),
	SafePath  UMETA(DisplayName="SafePath Puzzle")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionNameReplicated, const FName&, Name);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyPhaseChanged, ELobbyPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSelectionsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlarmChanged, bool, bNewState);


UCLASS()
class COOPGAME_API ACoopGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	//--- Delegates ---
	FOnSessionNameReplicated OnSessionNameReplicated;
	FOnPlayerSelectionsChanged OnPlayerSelectionsChanged;
	FOnAlarmChanged OnAlarmChanged;
	FOnLobbyPhaseChanged OnLobbyPhaseChanged;

	//--- Lobby & Session ---
	
	/** Returns the current session name. */
	FORCEINLINE FName GetSessionName() const { return SessionName; };
	
	/** Sets the session name (called by GameMode on the server). */
	void SetSessionName(const FName& NewSessionName);

	/** Checks player readiness and advances the lobby phase if conditions are met. Server only. */
	void CheckAndAdvanceLobbyPhase();

	/** Returns the current phase of the lobby. */
	UFUNCTION(BlueprintPure, Category="Lobby")
	ELobbyPhase GetCurrentLobbyPhase() const { return CurrentLobbyPhase; }

	/** Checks if the game can be started (all players ready and have selected a character). */
	UFUNCTION(BlueprintCallable, Category="Lobby")
	bool CanStartGame() const;

	//--- Character Selection ---

	/** Returns the array of all available character definitions. */
	const TArray<TObjectPtr<UCharacterDefinition>>& GetCharacters() const;
	
	/** Checks if a specific character has been selected by any player. */
	bool IsCharacterSelected(const UCharacterDefinition* CharacterToCheck) const;

	/** Called on the server to update a player's character choice. */
	void UpdateCharacterSelection(const UCharacterDefinition* Selected, const UCharacterDefinition* Deselected, APlayerState* Player);

	/** Returns the number of players who have currently selected a character. */
	UFUNCTION(BlueprintCallable, Category="Lobby")
	int32 GetSelectedCharactersCount() const { return PlayerSelections.Num(); }
	
	/** Gets the character definition selected by a specific player. */
	const UCharacterDefinition* GetCharacterSelectedByPlayer(APlayerState* Player) const;
	
	/** Gets the PlayerState that has selected a specific character. */
	TObjectPtr<APlayerState> GetPlayerStateForSelectedCharacter(const UCharacterDefinition* Character) const;

	//--- Gameplay State ---
	
	UPROPERTY(Replicated)
	TObjectPtr<AHackerPlayerController> HackerPlayerController;
	
	UPROPERTY(Replicated)
	TObjectPtr<AAgentPlayerController> AgentPlayerController;
	
	UPROPERTY(Replicated)
	TArray<int8> CodePuzzleSolution = {1, 2, 3, 4};
	
	UPROPERTY(ReplicatedUsing = OnRep_IsAlarmActive)
	bool bIsAlarmActive = false;
	
	UPROPERTY(Replicated)
	int32 GameTimer;

	/** Sets the alarm state on the server. */
	UFUNCTION()
	void SetAlarm(bool bValue);

protected:
	/** Specifies properties to replicate. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//--- OnRep Functions (called on clients when replicated properties change) ---

	UFUNCTION()
	void OnRep_SessionName();

	UFUNCTION()
	void OnRep_LobbyPhase();

	UFUNCTION()
	void OnRep_PlayerSelections();

	UFUNCTION()
	void OnRep_IsAlarmActive();

private:
	//--- Replicated Properties ---
	/** The name of the current online session. */
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName SessionName;

	/** The current phase of the lobby (e.g., ReadyUp, CharacterSelection). */
	UPROPERTY(ReplicatedUsing = OnRep_LobbyPhase)
	ELobbyPhase CurrentLobbyPhase = ELobbyPhase::ReadyUp;

	/** Replicated array holding all current character selections. This is the source of truth. */
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelections)
	TArray<FPlayerCharacterSelection> PlayerSelections;
	
	//--- Non-Replicated Properties ---

	/** The list of all characters available for selection in this game. Set in Blueprint. */
	UPROPERTY(EditDefaultsOnly, Category = "Characters", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UCharacterDefinition>> Characters;

	/** Timer handle for the delay between all players being ready and the phase changing. */
	FTimerHandle PhaseTransitionTimerHandle;

	/** The maximum number of players for this game mode. */
	static constexpr int32 MaxPlayers = 2;

	//--- Private Helper Functions ---

	/** Called by the timer to officially switch to the character selection phase. */
	void StartCharacterSelectionPhase();

	/** Cancels the phase transition timer if it's active. */
	void CancelPhaseTransition();
};