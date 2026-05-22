// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterEntry.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class COOPGAME_API UCharacterEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void SetCharacterSelected(bool bIsSelected) const;
	void SetOwningPlayerName(const FString& PlayerName) const;
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;
	
	UPROPERTY(EditDefaultsOnly)
	FName IconMaterialParamName{ "Icon" };
	
	UPROPERTY(EditDefaultsOnly)
	FName SaturationMaterialParamName{ "Saturation" };
};
