// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerEntry.h"
#include "CoopGame/Core/CoopPlayerState.h"
#include "Components/TextBlock.h"

void ULobbyPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (RepresentedPlayerState.IsValid())
	{
		RepresentedPlayerState->OnPlayerReadyStateChanged.RemoveDynamic(this, &ULobbyPlayerEntry::HandlePlayerReadyStateChanged);
	}

	ACoopPlayerState* PlayerState = Cast< ACoopPlayerState>(ListItemObject);
	if (PlayerState)
	{
		PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
		PlayerState->OnPlayerReadyStateChanged.AddDynamic(this, &ULobbyPlayerEntry::HandlePlayerReadyStateChanged);
		HandlePlayerReadyStateChanged(PlayerState->IsReady());
	}
}
void ULobbyPlayerEntry::NativeOnEntryReleased()
{
	if (RepresentedPlayerState.IsValid())
	{
		RepresentedPlayerState->OnPlayerReadyStateChanged.RemoveDynamic(this, &ULobbyPlayerEntry::HandlePlayerReadyStateChanged);
	}
	IUserObjectListEntry::NativeOnEntryReleased();
}

void ULobbyPlayerEntry::HandlePlayerReadyStateChanged(bool bIsReady)
{
	if (ReadyStatusText)
	{
		if (bIsReady)
		{
			ReadyStatusText->SetText(FText::FromString(TEXT("READY")));
			ReadyStatusText->SetColorAndOpacity(FLinearColor::Green);
		}
		else
		{
			ReadyStatusText->SetText(FText::FromString(TEXT("NOT READY")));
			ReadyStatusText->SetColorAndOpacity(FLinearColor::White);
		}
	}
}