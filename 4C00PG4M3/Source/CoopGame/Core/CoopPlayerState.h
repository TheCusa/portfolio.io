// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CoopPlayerState.generated.h"

class UCharacterDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerReadyStateChanged, bool, bIsReady);

/**
 *
 */
UCLASS()
class COOPGAME_API ACoopPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// Select/deselect characters (modified to handle toggle)
	UFUNCTION(Server, Reliable)
	void Server_IssueCharacterPicked(const UCharacterDefinition* SelectedCharacter);

	// Get the currently selected character
	UFUNCTION(BlueprintCallable)
	const UCharacterDefinition* GetCurrentSelectedCharacter() const { return CurrentSelectedCharacter; }
	
	UFUNCTION(BlueprintCallable)
	UClass* GetPickedCharacterClass() const;
	
	// Set the character directly (for restore after travel)
	void SetCurrentSelectedCharacter(const UCharacterDefinition* Character);

	// Delegate for UI updates
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnPlayerReadyStateChanged OnPlayerReadyStateChanged;

	// Function to set the ready state from the client to the server
	UFUNCTION(Server, Reliable)
	void Server_SetIsReady(bool bNewReadyState);

	// Getter for the ready state
	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool IsReady() const { return bIsReady; }

private:
	// RPC implementation
	void Server_IssueCharacterPicked_Implementation(const UCharacterDefinition* SelectedCharacter);

	// Reference to the currently selected character (replicated)
	UPROPERTY(Replicated)
	const UCharacterDefinition* CurrentSelectedCharacter;

	// Replicated variable to store the ready state
	UPROPERTY(ReplicatedUsing = OnRep_IsReady)
	bool bIsReady = false;

	// OnRep function to broadcast the change
	UFUNCTION()
	void OnRep_IsReady();

protected:
	// Override for replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Override for preserve data during seamless travel
	virtual void CopyProperties(APlayerState* PlayerState) override;
};