// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameWidgetParentClass.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "CoopGame/SoundPuzzle/SoundDataAsset.h"
#include "HackerPianoWidget.generated.h"

/**
 * 
 */

struct FNoteVisualInfo
{
	int32 StaffIndex;   
	bool bIsFlat;       
	bool bIsSharp;
};


UCLASS()
class COOPGAME_API UHackerPianoWidget : public UGameWidgetParentClass
{
protected:
	virtual void NativeConstruct() override;
	
	GENERATED_BODY()

	UPROPERTY()
	TArray<UBorder*> NoteBorders;
	UPROPERTY()
	TArray<UImage*> NoteBoxes;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* NoteSequenceContainer;

	UPROPERTY(EditAnywhere, Category = "Piano Configuration")
	float GlobalNoteVerticalOffset = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugScaleInEditor = false;
	
public:
	virtual void NativePreConstruct() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* NoteTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* FlatNoteTexture;
	
	UFUNCTION()
	void SetNotes(const TArray<int8>& NoteSequence, FLinearColor NoteColor = FLinearColor::White);
	UFUNCTION()
	void CreateNotes(int Size, FLinearColor NoteColor = FLinearColor::White);
	UFUNCTION()
	void UpdateFade();
	UFUNCTION()
	void ResetTimer();
	float GetNoteOffset(Notes Note);
	static TOptional<FNoteVisualInfo> GetNoteVisualInfo(Notes Note);
	UFUNCTION()
	void FadeOutNotes();

private:
	FTimerHandle FadeHandle;
	UPROPERTY(EditAnywhere)
	float FadeDuration = 5.0f; // default value
	float FadeElapsed = 0.0f;
};
