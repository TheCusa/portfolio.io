// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGame/Widgets/DigitDisplayWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/Font.h"

void UDigitDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDigitDisplayWidget::SetDigits(const TArray<int8>& Guess, const TArray<int8>& Secret)
{

	CreateDigits(Guess.Num());

	FString CurrentGuess;
	for (int8 Digit : Guess)
	{
		CurrentGuess += FString::FromInt(Digit);
	}

	PreviousGuess = LastGuess;
	LastGuess = CurrentGuess;
	HistoryText->SetText(FText::FromString(PreviousGuess + TEXT("\n") + LastGuess));
	
	if (Guess.Num() != Secret.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Guess or Secret are different sizes!"));
		return;
	}
	
	// Initialize background colors to red
	TArray<FLinearColor> BackgroundColors;
	BackgroundColors.Init(FLinearColor::Red, Secret.Num());

	// Set green background for correct digits
	for (int32 i = 0; i < Secret.Num(); i++)
	{
		if (Guess[i] == Secret[i])
		{
			BackgroundColors[i] = FLinearColor::Green;
		}
	}
	
	for (int32 i = 0; i < Secret.Num(); i++)
	{
		if (BackgroundColors[i] == FLinearColor::Green) continue;
		// Check for correct digits in the wrong position
		for (int32 j = 0; j < Secret.Num(); j++)
		{
			if (i != j && BackgroundColors[j] != FLinearColor::Green && Guess[i] == Secret[j])
			{
				BackgroundColors[i] = FLinearColor(1.0f, 0.5f, 0.0f, 0.8f);
				break;
			}
		}
	}
	
	for (int32 i = 0; i < Secret.Num(); i++)
	{
		DigitBorders[i]->SetBrushColor(BackgroundColors[i]);
		DigitTextBoxes[i]->SetText(FText::AsNumber(Guess[i]));
	}
}

void UDigitDisplayWidget::CreateDigits(int Size)
{
	DigitsContainer1->ClearChildren();
	DigitBorders.Reset();
	DigitTextBoxes.Reset();
	
	for (int i = 0; i < Size; i++)
	{
		UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		UTextBlock* TextBox = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		
		TextBox->SetText(FText::FromString("0"));
		TextBox->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TextBox->SetJustification(ETextJustify::Center);
		// ReSharper disable once CppDeprecatedEntity
		
		if (Font)
		{
			TextBox->SetFont(FSlateFontInfo(Font, FontSize));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to find engine font asset: %s"), *Font->GetName());
		}
		
		Border->SetBrushColor(FLinearColor::Black);
		Border->SetHorizontalAlignment(HAlign_Center);
		Border->SetVerticalAlignment(VAlign_Center);
		Border->SetContent(TextBox);
		
		UHorizontalBoxSlot* BoxSlot = DigitsContainer1->AddChildToHorizontalBox(Border);
		BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

		DigitBorders.Add(Border);
		DigitTextBoxes.Add(TextBox);
	}
}
