// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginMenu.generated.h"

class UTextBlock;
class UEOSGameInstance;
class UWidgetSwitcher;
class UButton;

/**
 * 
 */
UCLASS()
class COOPGAME_API ULoginMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	TObjectPtr<UEOSGameInstance> GameInstance;
	
	// Widget Switcher
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ScreenSwitcher;

	//Widget 1 - Login
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoginBtn;
	
	//Widget 2 - Loading

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LoadingText;
		
	//Widget 3 - Main Menu

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UsernameText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionsBtn;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitBtn;

	UFUNCTION()
	void OnLoginClicked();

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void HandleLoginSuccess();

	UFUNCTION()
	void HandleLoginFailed(const FString& Error);
	
	void ShowLoginScreen();
	void ShowLoadingScreen();
	void ShowMainMenuScreen();
	void SetupMainMenuScreen();
};