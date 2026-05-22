// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopGame/FirstPuzzle/Keypad.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>
#include "Components/WidgetComponent.h"
#include "CoopGame/Core/GameModes/GameplayGameMode.h"
#include "CoopGame/Core/PlayerControllers/AgentPlayerController.h"
#include "CoopGame/Characters/CharacterParentClass.h"
#include "CoopGame/Widgets/KeypadWidget.h"
#include "CoopGame/Core/Puzzle/Utils.h"


// Sets default values
AKeypad::AKeypad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	// Create the mesh component and attach it to root component
	KeypadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	KeypadMesh->SetupAttachment(RootComponent);

	// Create and setup Widget Component
	KeypadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("KeypadWidgetComponent"));
	KeypadWidgetComponent->SetupAttachment(RootComponent);

	// Configure widget settings
	KeypadWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	KeypadWidgetComponent->SetVisibility(false);

	AgentPC = nullptr;
}

// Called when the game starts or when spawned
void AKeypad::BeginPlay()
{
	Super::BeginPlay();

	// "Subscribe" to a delegate event on KeypadWideg 
	// when the event occour, execute HandleWidgetCodeSent function
	if (KeypadWidgetComponent)
	{
		if (UUserWidget* UIWidget = KeypadWidgetComponent->GetUserWidgetObject())
		{
			if (UKeypadWidget* KeypadUI = Cast<UKeypadWidget>(UIWidget))
			{
				KeypadUI->OnCodeEntered.AddDynamic(this, &AKeypad::HandleWidgetCodeSent);
			}
		}
	}

	// Set Agent Player Controller
	ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState());
	AgentPC = GameStateRef->AgentPlayerController;

	if (HasAuthority())
	{
		GenerateNewCode();
	}
}

// Do something when player interact with this object
void AKeypad::ExecuteAction()
{
	if (!IsActive && AgentPC)
	{
		AgentPC->ServerRequestSaveCodeIntoGameState(this);
		LoadInputMode();
	}
	else
	{
		RestoreInputMode();
	}
}

void AKeypad::HandleWidgetCodeSent(const TArray<int8>& SentCode)
{
	if (Utils::CheckEquals(SentCode, Code))
	{
		RestoreInputMode();
	}
	AgentPC->SendArrayCode(SentCode, GameUserWidget::DigitDisplay);
	AgentPC->ServerRequestCheckCode(this, SentCode);
	UE_LOG(LogTemp, Warning, TEXT("CODE SENT"));

	// Hide UI and create a new code on server

}

void AKeypad::Server_CheckCode_Implementation(const TArray<int8>& Array)
{
	if (Utils::CheckEquals(Array, Code))
	{
		OpenDoor();
		
		if (HasAuthority())
		{
			GenerateNewCode();
		}

		for (AMovingDoor* const& TargetDoor : TargetDoors)
		{
			if (IsValid(TargetDoor))
			{
				Cast<ACoopGameState>(GetWorld()->GetGameState())->SetAlarm(false);
			}
		}
	}
}

void AKeypad::Server_SaveCodeIntoGameState_Implementation()
{
	ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState());
	if (GameStateRef)
	{
		GameStateRef->CodePuzzleSolution = Code;
		Utils::DebugShowCode(GameStateRef->CodePuzzleSolution, "CodePuzzleSolution: ");
	}
}

void AKeypad::LoadInputMode()
{
	IsActive = true;
	
	FInputModeGameAndUI InputMode;

	if (AgentPC)
	{
		ACharacterParentClass* CharRef = Cast<ACharacterParentClass>(AgentPC->GetPawn());
		CharRef->LockCharacterMovement();
		AgentPC->SetInputMode(InputMode);
		AgentPC->SetViewTargetWithBlend(this, 0.5f);
		CharRef->GetMesh()->SetVisibility(false);
		AgentPC->SetShowMouseCursor(true);
	}
}

void AKeypad::RestoreInputMode()
{
	//KeypadWidgetComponent->SetVisibility(false);
	IsActive = false;

	FInputModeGameOnly InputMode;

	if (AgentPC)
	{
		ACharacterParentClass* CharRef = Cast<ACharacterParentClass>(AgentPC->GetPawn());
		CharRef->UnlockCharacterMovement();
		AgentPC->SetInputMode(InputMode);
		AgentPC->SetViewTargetWithBlend(CharRef, 0.5f);
		CharRef->GetMesh()->SetVisibility(true);
		AgentPC->SetShowMouseCursor(false);
	}
}

void AKeypad::GenerateNewCode()
{
	Code.Empty();
	Code = Utils::GenerateCode(4);
	//Utils::DebugShowCode(Code, "CodeGenerated: ");
}

void AKeypad::OpenDoor()
{
	if (!HasAuthority())
		return;
	
	if (TargetDoors.Num() <= 0)
		return;

	for (AMovingDoor* const& TargetDoor : TargetDoors)
	{
		if (TargetDoor)
		{
			if (!TargetDoor->IsOpen())
			{
				TargetDoor->OpenDoor();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("TargetDoor not assigned!"));
		}
	}
}

void AKeypad::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AKeypad, Code);
}