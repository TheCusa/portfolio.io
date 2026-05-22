// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameWidgetParentClass.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "DigitDisplayWidget.generated.h"

/**
 * 
 */

class UFont;

UCLASS()
class COOPGAME_API UDigitDisplayWidget : public UGameWidgetParentClass
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetDigits(const TArray<int8>& Guess, const TArray<int8>& Secret);
	void CreateDigits(int Size);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TArray<UBorder*> DigitBorders;
	UPROPERTY()
	TArray<UTextBlock*> DigitTextBoxes;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* DigitsContainer1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HistoryText;

	FString LastGuess;
	FString PreviousGuess;
	
	UPROPERTY(EditDefaultsOnly, Category = "Font Setup")
	TObjectPtr<UFont> Font;
	UPROPERTY(EditDefaultsOnly, Category = "Font Setup")
	int32 FontSize = 72;
};
