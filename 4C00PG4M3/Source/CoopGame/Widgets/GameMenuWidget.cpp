// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenuWidget.h"
#include "Components/Button.h"
#include "CoopGame/EOS/EOSGameInstance.h"
#include "GameFramework/PlayerState.h"

void UGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	/*if (OptionsBtn)
	{
		OptionsBtn->OnClicked.AddDynamic(this, &UGameMenuWidget::OnOptionsClicked);
		
	}*/
	if (ExitBtn)
	{
		ExitBtn->OnClicked.AddDynamic(this, &UGameMenuWidget::OnExitClicked);
	}
}

void UGameMenuWidget::OnExitClicked()
{
	APlayerState* ExitingPlayerState = GetOwningPlayer()->GetPlayerState<APlayerState>();
	if (ExitingPlayerState)
	{
		UEOSGameInstance* GI = GetGameInstance<UEOSGameInstance>();
		GI->ReturnToMainMenu();
	}
}


