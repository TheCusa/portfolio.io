#include "GameplayGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "CoopGame/Core/CoopPlayerState.h"
#include "CoopGame/Core/CoopGameState.h"
#include "CoopGame/Characters/CharacterDefinition.h"
#include "CoopGame/Core/Puzzle/Utils.h"
#include "CoopGame/Characters/Agent/AgentCharacter.h"
#include "CoopGame/Core/PlayerControllers/AgentPlayerController.h"
#include "CoopGame/Core/PlayerControllers/CoopPlayerController.h"
#include "CoopGame/Core/PlayerControllers/HackerPlayerController.h"
#include "Kismet/GameplayStatics.h"

AGameplayGameMode::AGameplayGameMode()
{
	bUseSeamlessTravel = true;
	bIsHackerServer = true;
#ifdef UE_BUILD_DEVELOPMENT
	if (HackerPlayerController)
	{
		PlayerControllerClass = HackerPlayerController;
		UE_LOG(LogGameMode, Log, TEXT("Development: Set default PlayerController to HackerPlayerController Blueprint"));
	}
	else
	{
		PlayerControllerClass = ACoopPlayerController::StaticClass();
	}
	
#else
	PlayerControllerClass = ACoopPlayerController::StaticClass();
#endif
}

TSubclassOf<APlayerController> AGameplayGameMode::GetPlayerControllerClassForIndex(int32 PlayerIndex)
{
	if (bIsHackerServer)
	{
		switch (PlayerIndex)
		{
		case 0:
			return HackerPlayerController ?
			TSubclassOf<APlayerController>(HackerPlayerController) :
			TSubclassOf<APlayerController>(ACoopPlayerController::StaticClass());

		case 1:
			return AgentPlayerController ?
			TSubclassOf<APlayerController>(AgentPlayerController) :
			TSubclassOf<APlayerController>(ACoopPlayerController::StaticClass());
		
		default:
			return ACoopPlayerController::StaticClass();
		}
	}
	else
	{
		switch (PlayerIndex)
		{
		case 1:
			return HackerPlayerController ?
			TSubclassOf<APlayerController>(HackerPlayerController) :
			TSubclassOf<APlayerController>(ACoopPlayerController::StaticClass());

		case 0:
			return AgentPlayerController ?
			TSubclassOf<APlayerController>(AgentPlayerController) :
			TSubclassOf<APlayerController>(ACoopPlayerController::StaticClass());
		
		default:
			return ACoopPlayerController::StaticClass();
		}
	}
}

APlayerController* AGameplayGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
#ifdef UE_BUILD_DEVELOPMENT
	int32 CurrentPlayerCount = GetNumPlayers();
	TSubclassOf<APlayerController> ControllerClass = GetPlayerControllerClassForIndex(CurrentPlayerCount);

	UE_LOG(LogGameMode, Log, TEXT("SpawnPlayerController: Player %d using %s"),
		CurrentPlayerCount + 1, ControllerClass ? *ControllerClass->GetName() : TEXT("Default"));

	if (ControllerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.ObjectFlags |= RF_Transient;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		return GetWorld()->SpawnActor<APlayerController>(ControllerClass, SpawnParams);
	}
#endif

	return Super::SpawnPlayerController(InRemoteRole, Options);
}

void AGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	ACoopGameState* GameStateRef = GetGameState<ACoopGameState>();
	if (!GameStateRef) return;
		
	GameStateRef->GameTimer = GameDuration;
	StartCountdown(GameStateRef->GameTimer);

	// Debug logs
	UE_LOG(LogGameMode, Log, TEXT("HackerPlayerController Blueprint: %s"),
		HackerPlayerController ? *HackerPlayerController->GetName() : TEXT("NOT SET"));
	UE_LOG(LogGameMode, Log, TEXT("AgentPlayerController Blueprint: %s"),
		AgentPlayerController ? *AgentPlayerController->GetName() : TEXT("NOT SET"));
}

void AGameplayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (!NewPlayer) return;

	ACoopPlayerState* PlayerState = NewPlayer->GetPlayerState<ACoopPlayerState>();
	ACoopGameState* GameStateRef = GetGameState<ACoopGameState>();
	
	// Save PlayerController into GameState
	if (GameStateRef)
	{
		if (AHackerPlayerController* HackerPC = Cast<AHackerPlayerController>(NewPlayer))
		{
			GameStateRef->HackerPlayerController = HackerPC;
			UE_LOG(LogGameMode, Log, TEXT("Saved HackerPlayerController to GameState"));
		}
		else if (AAgentPlayerController* AgentPC = Cast<AAgentPlayerController>(NewPlayer))
		{
			GameStateRef->AgentPlayerController = AgentPC;
			UE_LOG(LogGameMode, Log, TEXT("Saved AgentPlayerController to GameState"));
		}
	}

	if (!PlayerState || PlayerState->GetCurrentSelectedCharacter()) return;

#ifdef UE_BUILD_DEVELOPMENT
	UCharacterDefinition* AssignedCharacter = nullptr;

	// Assign character based on controller type
	if (Cast<AHackerPlayerController>(NewPlayer))
	{
		AssignedCharacter = HackerCharacterDefinition;
		UE_LOG(LogGameMode, Log, TEXT("Development: HackerPlayerController assigned Hacker character"));
	}
	else if (Cast<AAgentPlayerController>(NewPlayer))
	{
		AssignedCharacter = AgentCharacterDefinition;
		UE_LOG(LogGameMode, Log, TEXT("Development: AgentPlayerController assigned Agent character"));
	}
	else
	{
		// Fallback for generic controllers
		AssignedCharacter = (DevelopmentPlayerCounter == 0) ? HackerCharacterDefinition : AgentCharacterDefinition;
	}

	if (AssignedCharacter)
	{
		PlayerState->SetCurrentSelectedCharacter(AssignedCharacter);
	}
	DevelopmentPlayerCounter++;
#endif
}

UClass* AGameplayGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (!InController) return Super::GetDefaultPawnClassForController_Implementation(InController);

	ACoopPlayerState* PlayerState = InController->GetPlayerState<ACoopPlayerState>();
	if (!PlayerState) return Super::GetDefaultPawnClassForController_Implementation(InController);

	// First priority: use picked character class
	if (UClass* PickedClass = PlayerState->GetPickedCharacterClass())
	{
		UE_LOG(LogGameMode, Log, TEXT("Using character class from PlayerState: %s"), *PickedClass->GetName());
		return PickedClass;
	}

	// Second priority: use controller-specific character
	if (Cast<AHackerPlayerController>(InController) && HackerCharacterDefinition)
		return HackerCharacterDefinition->CharacterClass;
	
	if (Cast<AAgentPlayerController>(InController) && AgentCharacterDefinition)
		return AgentCharacterDefinition->CharacterClass;

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

AActor* AGameplayGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	if (!Player) return Super::FindPlayerStart_Implementation(Player, IncomingName);

	ACoopPlayerState* CoopPS = Player->GetPlayerState<ACoopPlayerState>();
	if (!CoopPS) return Super::FindPlayerStart_Implementation(Player, IncomingName);

	// Determine spawn tag based on character or controller type
	FName TargetTag = NAME_None;
	const UCharacterDefinition* PickedDefinition = CoopPS->GetCurrentSelectedCharacter();
	
	if (PickedDefinition == HackerCharacterDefinition || Cast<AHackerPlayerController>(Player))
		TargetTag = FName("HackerSpawn");
	else if (PickedDefinition == AgentCharacterDefinition || Cast<AAgentPlayerController>(Player))
		TargetTag = FName("AgentSpawn");

	// Find spawn point with matching tag
	if (TargetTag != NAME_None)
	{
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			APlayerStart* StartPoint = *It;
			if (StartPoint && StartPoint->PlayerStartTag == TargetTag)
			{
				UE_LOG(LogGameMode, Log, TEXT("Found spawn point with tag '%s'"), *TargetTag.ToString());
				return StartPoint;
			}
		}
	}

	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

TSubclassOf<APlayerController> AGameplayGameMode::GetPlayerControllerClassToSpawnForSeamlessTravel(APlayerController* PreviousPlayerController)
{
	if (PreviousPlayerController)
	{
		if (ACoopPlayerState* CoopPS = PreviousPlayerController->GetPlayerState<ACoopPlayerState>())
		{
			FString CharacterName = CoopPS->GetCurrentSelectedCharacter()->CharacterName.ToString();
			UE_LOG(LogGameMode, Log, TEXT("GetPlayerControllerClassToSpawnForSeamlessTravel for character: %s"), *CharacterName);

			if (CharacterName.Equals("Hacker"))
			{
				return HackerPlayerController;
			}
			if (CharacterName.Equals("Agent"))
			{
				return AgentPlayerController;
			}
		}
	}
	return ACoopPlayerController::StaticClass();
}

void AGameplayGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	ACoopGameState* GameStateRef = GetGameState<ACoopGameState>();
	if (!C || !GameStateRef) return;

	// Restore controller references after seamless travel
	if (AHackerPlayerController* HackerPC = Cast<AHackerPlayerController>(C))
	{
		GameStateRef->HackerPlayerController = HackerPC;
		UE_LOG(LogGameMode, Log, TEXT("Seamless travel: Restored HackerPlayerController reference"));
	}
	else if (AAgentPlayerController* AgentPC = Cast<AAgentPlayerController>(C))
	{
		GameStateRef->AgentPlayerController = AgentPC;
		UE_LOG(LogGameMode, Log, TEXT("Seamless travel: Restored AgentPlayerController reference"));
	}
}

void AGameplayGameMode::StartCountdown(int32 Duration)
{
	if (ACoopGameState* GameStateRef = GetGameState<ACoopGameState>())
	{
		// Tick every second
		GameStateRef->GameTimer = Duration;
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AGameplayGameMode::TickCountdown, 1.0f, true);
	}
}

void AGameplayGameMode::TickCountdown()
{
	ACoopGameState* GameStateRef = Cast<ACoopGameState>(GetWorld()->GetGameState());
	if (GameStateRef && GameStateRef->GameTimer > 0)
	{
		GameStateRef->GameTimer--;
		// uncomment to print remaining timer on screen
		//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("GameTime: %d"), GameStateRef->GameTimer));
		if (GameStateRef->GameTimer <= 0)
		{
			GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
			OnTimerExpired();
#if !UE_BUILD_SHIPPING
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, TEXT("Finish Countdown Timer"));
#endif
		}
	}
}

void AGameplayGameMode::OnTimerExpired()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ACoopPlayerController* PC = Cast<ACoopPlayerController>(*It))
		{
			PC->Client_GameOver();
		}
	}
	
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}
