// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserGroupRotating.generated.h"

class URotatingMovementComponentInterp;
class ALaserHandler;

UENUM(BlueprintType)
enum class ELaserType : uint8
{
	Real UMETA(DisplayName = "Real Laser"),
	Fake UMETA(DisplayName = "Fake Laser")
};

USTRUCT(BlueprintType)
struct FLaserSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELaserType Type = ELaserType::Real;
};

UCLASS()
class COOPGAME_API ALaserGroupRotating : public AActor
{
	GENERATED_BODY()

public:
	ALaserGroupRotating();
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RootPivot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URotatingMovementComponentInterp* RotationComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Laser Classes")
	TSubclassOf<AActor> RealLaserClass;

	UPROPERTY(EditDefaultsOnly, Category = "Laser Classes")
	TSubclassOf<AActor> FakeLaserClass;

	// LAYOUT SETTINGS 
	UPROPERTY(EditAnywhere, Category = "Layout")
	TArray<FLaserSlot> LaserPattern;

	UPROPERTY(EditAnywhere, Category = "Layout")
	float Radius = 300.0f;

	// ONLY EDITOR
	UPROPERTY(EditDefaultsOnly, Category = "Editor Preview")
	UStaticMesh* PreviewMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Editor Preview")
	UMaterialInterface* PreviewMaterialReal;
	
	UPROPERTY(EditDefaultsOnly, Category = "Editor Preview")
	UMaterialInterface* PreviewMaterialFake;

protected:
	virtual void BeginPlay() override;
	FVector CalculateLaserOffset(int32 Index, int32 TotalCount, float CurrentRadius);	
	
private:	
	UPROPERTY()
	UInstancedStaticMeshComponent* PreviewRealISMC;
    
	UPROPERTY()
	UInstancedStaticMeshComponent* PreviewFakeISMC;
};