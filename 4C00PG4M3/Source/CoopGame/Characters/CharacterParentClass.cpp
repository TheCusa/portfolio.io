// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterParentClass.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame/FirstPuzzle/InteractableActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "Components/WidgetInteractionComponent.h"
#include "CoopGame/Core/PlayerControllers/CoopPlayerController.h"
#include "CoopGame/Widgets/GameMenuWidget.h"
#include  "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

// Sets default values
ACharacterParentClass::ACharacterParentClass()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
	WidgetInteractionComp->SetupAttachment(RootComponent);
	WidgetInteractionComp->InteractionSource = EWidgetInteractionSource::Mouse;
}

void ACharacterParentClass::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACharacterParentClass::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACharacterParentClass::Interact()
{
	ShowUIPrompt();

	if (NearbyInteractableObject && NearbyInteractableObject->IsA(AInteractableActor::StaticClass()))
	{
		(Cast<AInteractableActor>(NearbyInteractableObject))->ExecuteAction();
	}

	HideUIPrompt();
}

// Called when the game starts or when spawned
void ACharacterParentClass::BeginPlay()
{
	Super::BeginPlay();
	// Create prompt UI widget, but not show it
	if (InteractUIWidgetClass)
	{
		InteractUIWidget = CreateWidget<UUserWidget>(GetWorld(), InteractUIWidgetClass);
		if (InteractUIWidget)
		{
			InteractUIWidget->AddToViewport();
			HideUIPrompt();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractWidgetClass not assigned!"));
	}
}

// Called every frame
void ACharacterParentClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacterParentClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Add Input Mapping Context
	// Load Interaction Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(GameplayMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterParentClass::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterParentClass::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACharacterParentClass::Interact);
	
		//Click
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &ACharacterParentClass::OnLeftClickPressed);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &ACharacterParentClass::OnLeftClickReleased);

		//OpenMenu
		EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Started, this, &ACharacterParentClass::OpenMenu);
	
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void ACharacterParentClass::Jump()
{
	Super::Jump();
	if (JumpSound && CanJump())
	{
		UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
	}
}

void ACharacterParentClass::OnLeftClickPressed()
{
	if (WidgetInteractionComp)
	{
		WidgetInteractionComp->PressPointerKey(EKeys::LeftMouseButton);
	}
}

void ACharacterParentClass::OnLeftClickReleased()
{
	if (WidgetInteractionComp)
	{
		WidgetInteractionComp->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

void ACharacterParentClass::OpenMenu()
{
	APlayerController* PlayerController = Cast<ACoopPlayerController>(GetController());
	if (PlayerController)
	{
		if (bIsMenuOpen)
		{
			bIsMenuOpen = false;
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(false);
			GameMenuWidget->RemoveFromParent();
			UnlockCharacterMovement();
		}
		else
		{
			bIsMenuOpen = true;
			FInputModeGameAndUI InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(true);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			GameMenuWidget = CreateWidget<UGameMenuWidget>(GetWorld(), GameMenuWidgetClass);
			GameMenuWidget->AddToViewport(100);
			LockCharacterMovement();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

// Set the player's most nearby object. This function will be called by the instance of the object closest to the player
void ACharacterParentClass::SetNearbyInteractableObject(AActor* InteractableObject)
{
	NearbyInteractableObject = InteractableObject;

	// Check if any of these objects are nullptr and if implements AInteractableActor class
	if (!InteractableObject || !InteractUIWidget || !(InteractableObject->IsA(AInteractableActor::StaticClass())))
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR INTERACTABLE OBJECT REFERENCE"));
		return;
	}

	// Get object's prompt text
	FText InteractionText;
	AInteractableActor* InteractableActor = Cast<AInteractableActor>(InteractableObject);
	if (InteractableActor)
	{
		InteractionText = InteractableActor->GetUIPromptText();
		//UE_LOG(LogTemp, Warning, TEXT("TEXT OK"));
	}

	// Update UI widget
	UTextBlock* TextBlock = Cast<UTextBlock>(InteractUIWidget->GetWidgetFromName(TEXT("TextBlock_Prompt")));
	if (TextBlock)
	{
		TextBlock->SetText(InteractionText);
		//UE_LOG(LogTemp, Warning, TEXT("TEXTBLOCK OK"));
	}

	ShowUIPrompt();
}

// Clear the player's nearby object. This function will be called by the player instance of the object closest to the player
void ACharacterParentClass::ClearNearbyInteractableObject(AActor* InteractableObject)
{
	if (NearbyInteractableObject == InteractableObject)
	{
		NearbyInteractableObject = nullptr;

		HideUIPrompt();
	}
}

void ACharacterParentClass::LockCharacterMovement() const
{
	const_cast<ACharacterParentClass*>(this)->Server_StopCharacterMovement();

	if (Subsystem)
	{
		Subsystem->RemoveMappingContext(GameplayMappingContext);	// Remove gameplay mapping context
		Subsystem->AddMappingContext(UIMappingContext, 1);			// Add UI mapping mapping context
	}
}

void ACharacterParentClass::UnlockCharacterMovement() const
{
	if (Subsystem)
	{
		Subsystem->RemoveMappingContext(UIMappingContext);			// Remove UI mapping context
		Subsystem->AddMappingContext(GameplayMappingContext, 0);	// Add gameplay mapping context
	}
}

void ACharacterParentClass::ShowUIPrompt()
{
	if (InteractUIWidget && !(InteractUIWidget->IsVisible()))
	{
		InteractUIWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACharacterParentClass::HideUIPrompt()
{
	if (InteractUIWidget && InteractUIWidget->IsVisible())
	{
		InteractUIWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}


void ACharacterParentClass::Server_StopCharacterMovement_Implementation()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
	}
}