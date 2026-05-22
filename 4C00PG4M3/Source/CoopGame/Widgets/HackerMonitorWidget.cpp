// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGame/Widgets/HackerMonitorWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "DigitDisplayWidget.h"
#include "HackerLaserPuzzleWidget.h"
#include "HackerPianoWidget.h"

void UHackerMonitorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CreateAndAddWidgets();

	//Binding all the buttons
	if (KeypadButton)
	{
		KeypadButton->OnPressed.AddDynamic(this, &UHackerMonitorWidget::OnKeypadButtonPressed);
	}
	if (CameraButton)
	{
		CameraButton->OnPressed.AddDynamic(this, &UHackerMonitorWidget::OnCameraButtonPressed);
	}
	if (PianoButton)
	{
		PianoButton->OnPressed.AddDynamic(this, &UHackerMonitorWidget::OnPianoButtonPressed);
	}
	if (ExitButton)
	{
		ExitButton->OnReleased.AddDynamic(this, &UHackerMonitorWidget::OnExitButtonReleased);
		ExitButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UHackerMonitorWidget::OnKeypadButtonPressed()
{
	SwitchToWidget(0);
}

void UHackerMonitorWidget::OnCameraButtonPressed()
{
	Cast<UHackerLaserPuzzleWidget>(HackerLaserPuzzleWidgetInstance)->InitializeCameras();
	SwitchToWidget(1);
}

void UHackerMonitorWidget::OnPianoButtonPressed()
{
	SwitchToWidget(2);
}

void UHackerMonitorWidget::OnExitButtonReleased()
{
	ExitButton->SetVisibility(ESlateVisibility::Collapsed);
	WidgetSwitcher->SetVisibility(ESlateVisibility::Collapsed);
}

void UHackerMonitorWidget::SwitchToWidget(int32 WidgetIndex)
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
		WidgetSwitcher->SetActiveWidgetIndex(WidgetIndex);
	}
	if (ExitButton)
	{
		ExitButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UHackerMonitorWidget::CreateAndAddWidgets()
{
    if (!WidgetSwitcher) return;

    if (DigitDisplayWidget)
    {
        DigitDisplayWidgetInstance = CreateWidget<UDigitDisplayWidget>(this, DigitDisplayWidget);

        if (DigitDisplayWidgetInstance)
        {
            WidgetSwitcher->AddChild(DigitDisplayWidgetInstance);
        }
    }

    if (HackerLaserPuzzleWidget)
    {
        HackerLaserPuzzleWidgetInstance = CreateWidget<UHackerLaserPuzzleWidget>(this, HackerLaserPuzzleWidget);
        if (HackerLaserPuzzleWidgetInstance)
        {
            WidgetSwitcher->AddChild(HackerLaserPuzzleWidgetInstance);
        }
    }

	
	if (HackerPianoWidget)
    {
        HackerPianoWidgetInstance = CreateWidget<UHackerPianoWidget>(this, HackerPianoWidget);
        if (HackerPianoWidgetInstance)
        {
            WidgetSwitcher->AddChild(HackerPianoWidgetInstance);
        }
    }
}

UWidgetSwitcher* UHackerMonitorWidget::GetWidgetSwitcher()
{
	return WidgetSwitcher;
}
