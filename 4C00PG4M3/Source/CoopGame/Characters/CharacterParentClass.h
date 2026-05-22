// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CharacterParentClass.generated.h"

class UGameMenuWidget;
struct FInputActionValue;
class UEnhancedInputLocalPlayerSubsystem;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UUserWidget;
class UWidgetInteractionComponent;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class COOPGAME_API ACharacterParentClass : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Widget Interaction Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetInteractionComponent> WidgetInteractionComp;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> GameplayMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Navigation Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> UIMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Navigation Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Navigation Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> OpenMenuAction;
	
	// Nearby object reference
	UPROPERTY()
	TObjectPtr<AActor> NearbyInteractableObject;

	// Widget instance
	UPROPERTY()
	TObjectPtr<UUserWidget> InteractUIWidget;

	UPROPERTY()
	TObjectPtr<UGameMenuWidget> GameMenuWidget;

	// Local player enhanced input subsystem reference
	UPROPERTY()
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem;

	UPROPERTY(EditAnywhere, Category = "SFX", meta = (AllowPrivateAccess = "true"))
	USoundBase* JumpSound;

	void ShowUIPrompt();
	void HideUIPrompt();

	bool bIsMenuOpen = false;
protected:
	// Widget BP class reference. Assignable from deatils panel
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InteractUIWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGameMenuWidget> GameMenuWidgetClass;
	
	// References to UI Mapping context and Input Actions

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	/** Called for interact input */
	void Interact();
	/** Called for Jump input */
	void Jump();

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;
	
	/** Called for click input */
	void OnLeftClickPressed();
	void OnLeftClickReleased();

	/** Called for open menu*/
	void OpenMenu();

public:
	
	// Sets default values for this character's properties
	ACharacterParentClass();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Set the player's most nearby object.
	void SetNearbyInteractableObject(AActor* InteractableObject);
	// Clear the player's nearby object.
	void ClearNearbyInteractableObject(AActor* InteractableObject);

	void LockCharacterMovement() const;
	void UnlockCharacterMovement() const;

	// Stop character movemnet silulation on server
	UFUNCTION(Server, Reliable)
	void Server_StopCharacterMovement();
};
