// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginMenu.h"
#include "CoopGame/EOS/EOSGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"


void ULoginMenu::NativeConstruct()
{
	Super::NativeConstruct();
	GameInstance = GetGameInstance<UEOSGameInstance>();

	// Login
	LoginBtn->OnClicked.AddDynamic(this, &ULoginMenu::OnLoginClicked);
	
	// Main Menu
	HostBtn->OnClicked.AddDynamic(this, &ULoginMenu::OnHostClicked);
	//OptionsBtn->OnClicked.AddDynamic(this, &ULoginMenu::OptionsBtnClicked);
	QuitBtn->OnClicked.AddDynamic(this, &ULoginMenu::OnQuitClicked);
	
	if (GameInstance)
	{
		GameInstance->OnLoginSuccess.AddUObject(this, &ULoginMenu::HandleLoginSuccess);
		GameInstance->OnLoginFailed.AddUObject(this, &ULoginMenu::HandleLoginFailed);
		
		if (GameInstance->IsLoggedIn())
		{
			UE_LOG(LogTemp, Log, TEXT("User is already logged in. Showing main menu directly."));
			SetupMainMenuScreen();
		}
		else
		{
			ShowLoginScreen();
		}
	}
	else
	{
		// Fallback if GameInstance is invalid for some reason.
		ShowLoginScreen();
	}
}

void ULoginMenu::OnLoginClicked()
{
	if (GameInstance)
	{
		ShowLoadingScreen();
		GameInstance->Login();
	}
}

void ULoginMenu::OnHostClicked()
{
	if (GameInstance)
	{
		GameInstance->CreateSession();
	}
}

void ULoginMenu::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}


void ULoginMenu::HandleLoginSuccess()
{
	if (GameInstance)
	{
		UsernameText->SetText(FText::FromString(GameInstance->GetNickname()));
	}
	SetupMainMenuScreen();
}

void ULoginMenu::HandleLoginFailed(const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("Login failed UI Handler. Error: %s"), *Error);
	ShowLoginScreen();
}

void ULoginMenu::ShowLoginScreen()
{
	if (ScreenSwitcher)
	{
		ScreenSwitcher->SetActiveWidgetIndex(0);
	}
}
void ULoginMenu::ShowLoadingScreen()
{
	if (ScreenSwitcher)
	{
		ScreenSwitcher->SetActiveWidgetIndex(1);
	}
}
void ULoginMenu::ShowMainMenuScreen()
{
	if (ScreenSwitcher)
	{
		ScreenSwitcher->SetActiveWidgetIndex(2);
	}
}

void ULoginMenu::SetupMainMenuScreen()
{
	if (GameInstance)
	{
		UsernameText->SetText(FText::FromString(GameInstance->GetNickname()));
	}
	ShowMainMenuScreen();
}

