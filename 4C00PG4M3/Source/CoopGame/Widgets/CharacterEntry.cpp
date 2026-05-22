// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CoopGame/Characters/CharacterDefinition.h"

void UCharacterEntry::SetCharacterSelected(const bool bIsSelected) const
{
	if (IconImage && IconImage->GetDynamicMaterial())
	{
		IconImage->GetDynamicMaterial()->SetScalarParameterValue(SaturationMaterialParamName, bIsSelected ? 0.3f : 1.0f);
	}
	
	if (NameText)
	{
		const FLinearColor TextColor = bIsSelected ? FLinearColor(0.5f, 0.5f, 0.5f, 1.0f) : FLinearColor::White;
		NameText->SetColorAndOpacity(FSlateColor(TextColor));
	}
}

void UCharacterEntry::SetOwningPlayerName(const FString& PlayerName) const
{
	if (PlayerNameText)
	{
		if (!PlayerName.IsEmpty())
		{
			PlayerNameText->SetText(FText::FromString(PlayerName));
			PlayerNameText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PlayerNameText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UCharacterEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UCharacterDefinition* CharacterDefinition = Cast<UCharacterDefinition>(ListItemObject);
	if (CharacterDefinition)
	{
		NameText->SetText(FText::FromName(CharacterDefinition->CharacterName));
		if (IconImage && IconImage->GetDynamicMaterial())
		{
			IconImage->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, CharacterDefinition->Icon);
		}
	}
	SetOwningPlayerName(FString());
}
