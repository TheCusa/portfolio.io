// Fill out your copyright notice in the Description page of Project Settings.


#include "Piano.h"

#include "CoopGame/Core/GameModes/GameplayGameMode.h"
#include "CoopGame/Core/Puzzle/SoundPlatform.h"
#include "CoopGame/Core/Puzzle/Utils.h"
#include "CoopGame/FirstPuzzle/MovingDoor.h"
#include "CoopGame/SoundPuzzle/MelodiesDataTable.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APiano::APiano()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APiano::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		PuzzleSolution = PickRandomMelody();
		NoteArray.Init(Notes::None, PuzzleSolution.Num());
		TArray<int8> Temp = Utils::ConvertArrayToInt8(PuzzleSolution);
		UE_LOG(LogTemp, Warning, TEXT("Size PuzzleSolution: %d"), PuzzleSolution.Num());
		GameModeRef = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode());
		//Utils::DebugShowCode(Temp, "Piano Solution: ");
	}
}

void APiano::AddNote(Notes NoteID)
{
		NoteArray.Add(NoteID);

		if (NoteArray.Num() > PuzzleSolution.Num())
		{
			NoteArray.RemoveAt(0);
		}

		ResetStartTimer();

		for (int32 i = 0; i < NoteArray.Num(); ++i)
		{
			UE_LOG(LogTemp, Warning, TEXT("Array (%d) : %d"), i, static_cast<int32>(NoteArray[i])); //Debug Purpose
		}

		if (Utils::CheckEquals(NoteArray, PuzzleSolution))
		{
			UE_LOG(LogTemp, Warning, TEXT("Correct combination")) //Debug Purpose
			bIsPuzzleSolved = true;
			
			if (MovingDoor)
			{
				MovingDoor->OpenDoor();
			}
			if (GameModeRef)
			{
				GetWorld()->GetTimerManager().ClearTimer(GameModeRef->CountdownTimerHandle);
			}
			GetWorld()->GetTimerManager().ClearTimer(TimerHandler); //Deleting timer to avoid array erasing
		}
}

TArray<int8> APiano::GetPuzzleSolution()
{
	auto Solution = Utils::ConvertArrayToInt8(PuzzleSolution);
	return Solution;
}

void APiano::OnTimerFinished()
{
	NoteArray.Empty();
	NoteArray.Init(Notes::None, PuzzleSolution.Num());
	UE_LOG(LogTemp, Warning, TEXT("Clear Array")) //Debug purpose
}

void APiano::ResetStartTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandler);
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &APiano::OnTimerFinished, Timer, false);
}

TArray<Notes> APiano::PickRandomMelody()
{
	static const FString Context(TEXT("Melody Context"));
	TArray<FMelodyRow*> AllRows;
	if (MelodiesTable)
	{
		MelodiesTable->GetAllRows<FMelodyRow>(Context, AllRows);
	}

	if (AllRows.Num() == 0) return TArray<Notes>();
	
	int32 RandomIndex = FMath::RandRange(0, AllRows.Num()-1); //picking random row

	UE_LOG(LogTemp, Warning, TEXT("RandomIndex Picked: %d"), RandomIndex) //Debug Purpose

	FMelodyRow* RandomRow = AllRows[RandomIndex];
	
	return RandomRow->Notes;
}

void APiano::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APiano, PuzzleSolution);
}