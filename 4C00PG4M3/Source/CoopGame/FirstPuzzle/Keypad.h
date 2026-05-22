// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopGame/FirstPuzzle/InteractableActor.h"
#include "MovingDoor.h"
#include "Keypad.generated.h"

class AAgentPlayerController;
class UWidgetComponent;

UCLASS()
class COOPGAME_API AKeypad : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeypad();

	// Inherited functions from AInteractableActor class
	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAction() override;

	// RPC call for check code solution
	UFUNCTION(Server, Reliable)
	void Server_CheckCode(const TArray<int8>& EnteredCode);

	// RPC call for save code into gamestate
	UFUNCTION(Server, Reliable)
	void Server_SaveCodeIntoGameState();

	// Static mesh reference. Assignable in BP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* KeypadMesh;

	// UI Keypad Widget Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* KeypadWidgetComponent;

	// Target actor reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TARGET DOOR REFERENCE")
	TArray<AMovingDoor*> TargetDoors;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	TArray<int8> Code;
	bool IsActive = false;

	UPROPERTY()
	AAgentPlayerController* AgentPC;

	void LoadInputMode();
	void RestoreInputMode();
	void GenerateNewCode();
	void OpenDoor();

	UFUNCTION()
	void HandleWidgetCodeSent(const TArray<int8>& SentCode);
};