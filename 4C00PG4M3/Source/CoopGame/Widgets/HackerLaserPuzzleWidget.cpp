// Fill out your copyright notice in the Description page of Project Settings.


#include "HackerLaserPuzzleWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CoopGame/Core/Puzzle/SecurityCamera.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CoopGame/Core/CameraManager.h"
#include "CoopGame/Core/LevelStreamingManager.h"
#include "CoopGame/Core/PlayerControllers/HackerPlayerController.h"

void UHackerLaserPuzzleWidget::InitializeCameras()
{

	if (!HackerPlayerController->StreamingManager->bHasInitializedCameraFeed)
	{
		CameraManager = Cast<ACameraManager>(UGameplayStatics::GetActorOfClass(this, ACameraManager::StaticClass()));
		if (!CameraManager) return;
		SecurityCameras.Empty();

		for (ASecurityCamera* SecurityCamera : CameraManager->CurrentLevelCameras)
		{
			if (SecurityCamera)
			{
				SecurityCameras.Add(SecurityCamera);
			}
		}	
		SecurityCameras.Sort([](const ASecurityCamera& A, const  ASecurityCamera& B)
		{
			return A.SortingOrder < B.SortingOrder;
		});
	
		CameraFeedIndex = 0;

		if (CameraFeedImage)
		{
			CameraFeedMaterial = CameraFeedImage->GetDynamicMaterial();
		}
		HackerPlayerController->Server_UpdateFeed();
		SetFeed();
	}
}

void UHackerLaserPuzzleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PrevCameraButton->OnPressed.AddDynamic(this, &UHackerLaserPuzzleWidget::OnPrevCameraPressed);
	NextCameraButton->OnPressed.AddDynamic(this, &UHackerLaserPuzzleWidget::OnNextCameraPressed);
	HackerPlayerController = Cast<AHackerPlayerController>(GetOwningPlayer());
}

void UHackerLaserPuzzleWidget::NativeDestruct()
{
	Super::NativeDestruct();
	for (ASecurityCamera* Camera : SecurityCameras)
	{
		if (Camera)
		{
			Camera->SetActive(false);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("HackerLaserPuzzleWidget DESTRUCTED!"));
	
}

void UHackerLaserPuzzleWidget::SetFeed()
{
	if (SecurityCameras.Num() == 0 || !CameraFeedMaterial) return;
	
	if (CameraFeedIndex < 0)
	{
		CameraFeedIndex = SecurityCameras.Num() - 1;
	}
	else if (CameraFeedIndex >= SecurityCameras.Num())
	{
		CameraFeedIndex = 0;
	}

	ShownCameraText->SetText(FText::FromString(FString::FromInt(CameraFeedIndex + 1)));

	for (ASecurityCamera* Camera : SecurityCameras)
	{
		if (Camera)
		{
			Camera->SetActive(false);
		}
		else
		{
			return;
		}
	}
	SecurityCameras[CameraFeedIndex]->SetActive(true);
	if (SecurityCameras[CameraFeedIndex]->CameraCaptureComponent && SecurityCameras[CameraFeedIndex]->CameraCaptureComponent->TextureTarget)
	{
		CameraFeedMaterial->SetTextureParameterValue(TEXT("CameraTexture"), SecurityCameras[CameraFeedIndex]->CameraCaptureComponent->TextureTarget);
	}

	UE_LOG(LogTemp, Display, TEXT("SetFeed to Camera Index: %i, using Dynamic Material"), CameraFeedIndex);
}

void UHackerLaserPuzzleWidget::OnLeftKeyPressed()
{
	CameraFeedIndex--;
	SetFeed();
}

void UHackerLaserPuzzleWidget::OnRightKeyPressed()
{
	CameraFeedIndex++;
	SetFeed();
}

void UHackerLaserPuzzleWidget::OnNextCameraPressed()
{
	CameraFeedIndex++;
	SetFeed();
	InitializeCameras();
}

void UHackerLaserPuzzleWidget::OnPrevCameraPressed()
{
	CameraFeedIndex--;
	SetFeed();
	InitializeCameras();
}





