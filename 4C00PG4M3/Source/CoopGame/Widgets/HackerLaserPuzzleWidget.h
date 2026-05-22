// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameWidgetParentClass.h"
#include "HackerLaserPuzzleWidget.generated.h"


class UImage;
class UTextBlock;
class UButton;
class ASecurityCamera;
class UMaterialInstanceDynamic;
class ACameraManager;
class ALevelStreamingManager;
class AHackerPlayerController;
/**
 * 
 */
UCLASS()
class COOPGAME_API UHackerLaserPuzzleWidget : public UGameWidgetParentClass
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void InitializeCameras();
private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CameraFeedImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ShownCameraText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> NextCameraButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> PrevCameraButton;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CameraFeedMaterial;
	
	UPROPERTY()
	TArray<TObjectPtr<ASecurityCamera>> SecurityCameras;

	int32 CameraFeedIndex;
	UPROPERTY()
	ACameraManager* CameraManager;
	UPROPERTY()
	AHackerPlayerController* HackerPlayerController;

	UFUNCTION(BlueprintCallable, Category="Security Feeds")
	void SetFeed();
	
	virtual void OnLeftKeyPressed() override;
	virtual void OnRightKeyPressed() override;
	
	UFUNCTION()
	void OnNextCameraPressed();
	UFUNCTION()
	void OnPrevCameraPressed();
};
