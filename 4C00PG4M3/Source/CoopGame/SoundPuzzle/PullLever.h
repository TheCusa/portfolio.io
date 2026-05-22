#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "CoopGame/FirstPuzzle/InteractableActor.h"
#include "PullLever.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class APiano;
class ACoopGameState;
class AAgentPlayerController;
class UCurveFloat;

UCLASS()
class COOPGAME_API APullLever : public AInteractableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APullLever();

	//Inherited functions from IInteractableObjectInterface
	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAction() override;

	// Called every frame - only enabled when timeline is active
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Timeline callback functions
	UFUNCTION()
	void TimelineUpdate(float Alpha) const;

	UFUNCTION()
	void TimelineFinished();

private:
	//Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> StaticBaseMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> MovableLeverMesh;

	// Timeline Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTimelineComponent> LeverTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	TObjectPtr<UCurveFloat> LeverCurve;

	//References
	UPROPERTY(EditInstanceOnly, Category = "References")
	TObjectPtr<APiano> PianoReference;

	UPROPERTY()
	TObjectPtr<ACoopGameState> GameStateRef;

	UPROPERTY()
	TObjectPtr<AAgentPlayerController> AgentPlayerControllerRef;

	// Animation Properties
	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	float LeverPullAngle = 45.0f;

	UPROPERTY()
	FRotator InitialRotation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	bool bIsAnimating = false;

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;
	
	UPROPERTY(EditDefaultsOnly,Category = "Sounds")
	USoundBase* LeverSound;
};
