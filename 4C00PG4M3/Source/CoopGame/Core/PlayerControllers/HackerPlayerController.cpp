// Fill out your copyright notice in the Description page of Project Settings.


#include "HackerPlayerController.h"
#include "CoopGame/Characters/Hacker/HackerCharacter.h"
#include "CoopGame/Widgets/HackerPianoWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Widget.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetSwitcher.h"

#include "CoopGame/Widgets/HackerMonitorWidget.h"
#include "CoopGame/Core/Puzzle/HackerMonitor.h"
#include "CoopGame/Widgets/DigitDisplayWidget.h"
#include "CoopGame/Core/LevelStreamingManager.h"

void AHackerPlayerController::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHackerMonitor::StaticClass(), FoundActors);
    StreamingManager = Cast<ALevelStreamingManager>(UGameplayStatics::GetActorOfClass(this, ALevelStreamingManager::StaticClass()));

    if (FoundActors.Num() > 0)
    {
        HackerMonitor = Cast<AHackerMonitor>(FoundActors[0]);
        if (HackerMonitor)
        {
            UE_LOG(LogTemp, Warning, TEXT("HackerMonitor found successfully!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No HackerMonitor found in scene!"));
    }
}

void AHackerPlayerController::ReceiveArrayCode_Implementation(const TArray<int8>& Array, const GameUserWidget Widget)
{
    // Client is supposed to receive Arrays
    // if (!HasAuthority())
    //     return;
    
    if (!HackerMonitor) return;
    ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState());
    
    UWidgetComponent* WidgetComponent = HackerMonitor->FindComponentByClass<UWidgetComponent>();
    if (!WidgetComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("WidgetComponent not found"));
        return;
    }

    UHackerMonitorWidget* HackerMonitorWidget = Cast<UHackerMonitorWidget>(WidgetComponent->GetUserWidgetObject());
    if (!HackerMonitorWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot cast to HackerMonitorWidget"));
        return;
    }

    UWidget* ActiveWidget = HackerMonitorWidget->GetWidgetSwitcher()->GetActiveWidget();
    if (!ActiveWidget) return;

    switch (Widget)
    {
        case GameUserWidget::DigitDisplay:
        {
            UDigitDisplayWidget* DigitDisplayWidget = Cast<UDigitDisplayWidget>(ActiveWidget);
            if (!DigitDisplayWidget)
            {
                UE_LOG(LogTemp, Error, TEXT("Active widget is not DigitDisplayWidget"));
                return;
            }


            DigitDisplayWidget->SetDigits(Array, GameStateRef->CodePuzzleSolution);
            break;
        }

        case GameUserWidget::SoundPuzzleWidget:
        {
            UHackerPianoWidget* HackerSoundDisplay = Cast<UHackerPianoWidget>(ActiveWidget);
            if (!HackerSoundDisplay)
            {
                UE_LOG(LogTemp, Error, TEXT("Active widget is not HackerSoundDisplay"));
                return;
            }
            HackerSoundDisplay->SetNotes(Array); //DA MODIFICARE
            break;
        }
        case GameUserWidget::None:
            {
                UHackerPianoWidget* HackerSoundDisplay = Cast<UHackerPianoWidget>(ActiveWidget);
                if (!HackerSoundDisplay)
                {
                    UE_LOG(LogTemp, Error, TEXT("Active widget is not HackerSoundDisplay"));
                    return;
                }
                HackerSoundDisplay->SetNotes(Array, FLinearColor::Red);
                HackerMonitor->TriggerAudioSequenceForClient(this,Array);
                break;
            }
        default:
        {
            UE_LOG(LogTemp, Error, TEXT("DEFAULT"));
            break;
        }
    }
    
}
void AHackerPlayerController::Server_UpdateFeed_Implementation()
{
    StreamingManager->bHasInitializedCameraFeed = true;
}
