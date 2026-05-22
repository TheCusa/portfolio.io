// Fill out your copyright notice in the Description page of Project Settings.


#include "InvitePopup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CoopGame/EOS/EOSGameInstance.h"

void UInvitePopup::Setup(const FString& InviterName)
{
	if (InviteMessageText)
	{
		FString Message = FString::Printf(TEXT("%s INVITED YOU TO PLAY!"), *InviterName);
		InviteMessageText->SetText(FText::FromString(Message));
	}
}

void UInvitePopup::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstance = GetGameInstance<UEOSGameInstance>();

	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UInvitePopup::OnAcceptClicked);
	}
	if (DeclineButton)
	{
		DeclineButton->OnClicked.AddDynamic(this, &UInvitePopup::OnDeclineClicked);
	}
}

void UInvitePopup::OnAcceptClicked()
{
	if (GameInstance)
	{
		GameInstance->AcceptLastReceivedInvite();
	}
	// Close popup
	RemoveFromParent();
}

void UInvitePopup::OnDeclineClicked()
{
	// Close popup
	RemoveFromParent();
}