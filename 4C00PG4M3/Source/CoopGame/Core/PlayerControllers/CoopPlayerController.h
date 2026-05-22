// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoopGame/Core/CoopGameState.h"
#include "CoopPlayerController.generated.h"

class UGameWidgetParentClass;
class UDigitDisplayWidget;

UENUM(BlueprintType)
enum class GameUserWidget : uint8
{
	None        UMETA(DisplayName="None"),
	DigitDisplay UMETA(DisplayName="DigitDisplay"),
	HackerLaserMonitor UMETA(DisplayName="HackerLaserMonitor"),
	Keypad  UMETA(DisplayName="Keypad"),
	SoundPuzzleWidget UMETA(DisplayName = "SoundPuzzleWidget")
};


/**
 * 
 */
UCLASS()
class COOPGAME_API ACoopPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void Say(const FString& Message);
	
	UFUNCTION(Client, Reliable)
	void Client_GameOver();
protected:
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnRep_PlayerState"))
	void BP_OnRep_PlayerState();

	/** [CLIENT] -> [SERVER] Invia il messaggio al server per l'inoltro */
	UFUNCTION(Server, Reliable)
	void Server_SendMessage(const FString& Message);

	/** [SERVER] -> [CLIENT] Riceve il messaggio dal server e lo mostra a schermo */
	UFUNCTION(Client, Reliable)
	void Client_ReceiveMessage(const FString& Message, const FString& SenderName);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	UUserWidget* GameOverWidgetInstance;
public:
	//Debug for code checking
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowDebugMessage(const FString& Message);
	
};
