// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HackerMonitorWidget.generated.h"


class UButton;
class UWidgetSwitcher;
class UDigitDisplayWidget;
class UHackerLaserPuzzleWidget;
class UHackerPianoWidget;

/**
 * 
 */
UCLASS()
class COOPGAME_API UHackerMonitorWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* KeypadButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CameraButton;

	UPROPERTY(meta = (BindWidget))
	UButton* PianoButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UDigitDisplayWidget> DigitDisplayWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UHackerLaserPuzzleWidget> HackerLaserPuzzleWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UHackerPianoWidget> HackerPianoWidget;

	UPROPERTY()
	UUserWidget* DigitDisplayWidgetInstance;

	UPROPERTY()
	UUserWidget* HackerLaserPuzzleWidgetInstance;

	UPROPERTY()
	UUserWidget* HackerPianoWidgetInstance;

	UFUNCTION()
	void OnKeypadButtonPressed();

	UFUNCTION()
	void OnCameraButtonPressed();

	UFUNCTION()
	void OnPianoButtonPressed();
	
	UFUNCTION()
	void OnExitButtonReleased();
	
	UFUNCTION()
	void SwitchToWidget(int32 WidgetIndex);

private:
	void CreateAndAddWidgets();

public:
	UWidgetSwitcher* GetWidgetSwitcher();
	
};
