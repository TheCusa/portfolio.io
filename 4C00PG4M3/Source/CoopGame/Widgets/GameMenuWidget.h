// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class COOPGAME_API UGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> OptionsBtn;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ExitBtn;

	/*UFUNCTION()
	void OnOptionsClicked();*/

	UFUNCTION()
	void OnExitClicked();

protected:
	virtual void NativeConstruct() override;
};
