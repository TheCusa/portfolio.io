// Fill out your copyright notice in the Description page of Project Settings.

#include "HackerPianoWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBoxSlot.h"
#include "CoopGame/SoundPuzzle/SoundDataAsset.h"

void UHackerPianoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHackerPianoWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	//Only for Design in Editor
	if (IsDesignTime() && bShowDebugScaleInEditor)
	{
		TArray<int8> DebugSequence;
		const int32 StartIndex = static_cast<int32>(Notes::DO_1);
		const int32 EndIndex   = static_cast<int32>(Notes::SI_2);

		for (int32 i = StartIndex; i <= EndIndex; i++) 
		{
			DebugSequence.Add(static_cast<int8>(i));
		}
		SetNotes(DebugSequence);
	}
}

void UHackerPianoWidget::SetNotes(const TArray<int8>& NoteSequence, FLinearColor NoteColor)
{
	if (!IsValid(NoteTexture) || !IsValid(FlatNoteTexture))
	{
		UE_LOG(LogTemp, Error, TEXT("NoteTexture or FlatNoteTexture is NULL"));
		return;
	}

	CreateNotes(NoteSequence.Num(), NoteColor);

	for (int32 i = 0; i < NoteSequence.Num(); i++)
	{
		Notes NoteEnum = static_cast<Notes>(NoteSequence[i]);

		if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(NoteBorders[i]->Slot))
		{
			TOptional<FNoteVisualInfo> Info = GetNoteVisualInfo(NoteEnum);
			if (Info.IsSet())
			{
				const FNoteVisualInfo& V = Info.GetValue();
				NoteBoxes[i]->SetDesiredSizeOverride(V.bIsFlat ? FVector2D(128,128) : FVector2D(64,64));
				NoteBoxes[i]->SetBrushFromTexture(V.bIsFlat ? FlatNoteTexture : NoteTexture);
				NoteBoxes[i]->SetVisibility(ESlateVisibility::Visible);
				HSlot->SetPadding(FMargin(0.f, V.StaffIndex * (-GlobalNoteVerticalOffset), 0.f, 0.f));
			}
			else
			{
				NoteBoxes[i]->SetVisibility(ESlateVisibility::Collapsed);
				NoteBoxes[i]->SetBrushFromTexture(nullptr);
			}
		}
	}
}



void UHackerPianoWidget::CreateNotes(int Size, FLinearColor NoteColor)
{
	NoteSequenceContainer->ClearChildren();
	NoteBorders.Reset();
	NoteBoxes.Reset();

	for (int i = 0; i < Size; i++)
	{
		UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		UImage* NoteImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		
		NoteImage->SetBrushFromTexture(nullptr); 
		NoteImage->SetColorAndOpacity(NoteColor);
		Border->SetDesiredSizeScale(FVector2D(512.f, 512.f));
		Border->SetBrushColor(FLinearColor::Transparent);
		Border->SetHorizontalAlignment(HAlign_Center);
		Border->SetVerticalAlignment(VAlign_Center);
		Border->SetContent(NoteImage);

		UHorizontalBoxSlot* BoxSlot = NoteSequenceContainer->AddChildToHorizontalBox(Border);
		BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

		NoteBorders.Add(Border);
		NoteBoxes.Add(NoteImage);
	}
	FadeOutNotes();
}

void UHackerPianoWidget::FadeOutNotes()
{
	if (NoteBoxes.IsEmpty()) return;
	ResetTimer();
}

void UHackerPianoWidget::UpdateFade()
{

	FadeElapsed += 0.01f;
	float Alpha = 1.0f - (FadeElapsed / FadeDuration);

	for (UImage* NoteBox : NoteBoxes)
	{
		NoteBox->SetRenderOpacity(FMath::Clamp(Alpha, 0.0f, 1.0f));
	}

	if (FadeElapsed >= FadeDuration)
	{
		for (UImage*  NoteBox : NoteBoxes)
		{
			NoteBox->SetRenderOpacity(0.0f);
		}
	}
}

void UHackerPianoWidget::ResetTimer()
{
	FadeElapsed = 0.0f;
	GetWorld()->GetTimerManager().ClearTimer(FadeHandle);
	GetWorld()->GetTimerManager().SetTimer(FadeHandle, this, &UHackerPianoWidget::UpdateFade, 0.01f, true);
}

float UHackerPianoWidget::GetNoteOffset(Notes Note)
{
	int32 CenterIndex = static_cast<int32>(Notes::SI_1);
	int32 SemitoneOffset = static_cast<int32>(Note) - CenterIndex;
	return SemitoneOffset * -GlobalNoteVerticalOffset;
}

TOptional<FNoteVisualInfo> UHackerPianoWidget::GetNoteVisualInfo(Notes Note)
{
	// Static lookup table, initialized once
	static const TMap<Notes, FNoteVisualInfo> NoteInfoMap = {
		{ Notes::DO_1,   { -6, false, false } },
		{ Notes::REb_1,  { -5, true,  false } },
		{ Notes::RE_1,   { -5, false, false } },
		{ Notes::MIb_1,  { -4, true,  false } },
		{ Notes::MI_1,   { -4, false, false } },
		{ Notes::FA_1,   { -3, false, false } },
		{ Notes::SOLb_1, { -2, true,  false } },
		{ Notes::SOL_1,  { -2, false, false } },
		{ Notes::LAb_1,  { -1, true,  false } },
		{ Notes::LA_1,   { -1, false, false } },
		{ Notes::SIb_1,  {  0, true,  false } },
		{ Notes::SI_1,   {  0, false, false } },

		{ Notes::DO_2,   {  1, false, false } },
		{ Notes::REb_2,  {  2, true,  false } },
		{ Notes::RE_2,   {  2, false, false } },
		{ Notes::MIb_2,  {  3, true,  false } },
		{ Notes::MI_2,   {  3, false, false } },
		{ Notes::FA_2,   {  4, false, false } },
		{ Notes::SOLb_2, {  5, true,  false } },
		{ Notes::SOL_2,  {  5, false, false } },
		{ Notes::LAb_2,  {  6, true,  false } },
		{ Notes::LA_2,   {  6, false, false } },
		{ Notes::SIb_2,  {  7, true,  false } },
		{ Notes::SI_2,   {  7, false, false } }
	};
	
	if (const FNoteVisualInfo* Info = NoteInfoMap.Find(Note))
	{
		return *Info;
	}
	
	return {};
}





