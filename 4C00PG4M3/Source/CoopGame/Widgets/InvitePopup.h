// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvitePopup.generated.h"

class UButton;
class UTextBlock;
class UEOSGameInstance;

UCLASS()
class COOPGAME_API UInvitePopup : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const FString& InviterName);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InviteMessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AcceptButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeclineButton;

	UFUNCTION()
	void OnAcceptClicked();

	UFUNCTION()
	void OnDeclineClicked();

	UPROPERTY()
	TObjectPtr<UEOSGameInstance> GameInstance;
};