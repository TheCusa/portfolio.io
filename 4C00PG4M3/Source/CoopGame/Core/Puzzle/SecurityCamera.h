// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SecurityCamera.generated.h"

class ARenderTargetPoolManager;

UCLASS()
class COOPGAME_API ASecurityCamera : public AActor
{
	GENERATED_BODY()
private:

public:	
	// Sets default values for this actor's properties
	ASecurityCamera();
	
	void SetActive(bool bIsActive);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneCaptureComponent2D> CameraCaptureComponent;

	UPROPERTY(EditInstanceOnly)
	int8 SortingOrder;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> CameraStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Security Camera")
	TSubclassOf<ARenderTargetPoolManager> RenderTargetPoolManager = nullptr;
};
