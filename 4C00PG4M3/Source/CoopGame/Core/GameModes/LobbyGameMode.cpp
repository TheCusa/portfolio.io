// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "CoopGame/Core/CoopGameState.h"
#include "CoopGame/EOS/EOSGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

ALobbyGameMode::ALobbyGameMode()
{
    bUseSeamlessTravel = true;
}

void ALobbyGameMode::InitGameState()
{
    Super::InitGameState();
    ACoopGameState* CoopGameState = Cast<ACoopGameState>(GameState);
    UEOSGameInstance* EOSGameInstance = GetGameInstance<UEOSGameInstance>();

    if (CoopGameState && EOSGameInstance)
    {
        CoopGameState->SetSessionName(EOSGameInstance->GetCurrentSessionName());
    }
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        APlayerState* PlayerState = NewPlayer->PlayerState;
        if (PlayerState)
        {
            UEOSGameInstance* GI = GetGameInstance<UEOSGameInstance>();
            IOnlineSessionPtr SessionPtr = GI ? GI->GetSessionInterface() : nullptr;
            
            if (GI && SessionPtr.IsValid())
            {
                // We get the player's unique online ID from their PlayerState. This is the reliable source for this info on the server.
                const FUniqueNetIdRepl& PlayerId = PlayerState->GetUniqueId();
                FName SessionName = GI->GetCurrentSessionName();

                if (PlayerId.IsValid() && !SessionName.IsNone())
                {
                    //Tells the online subsystem (EOS) that this player is now an official member of the session.
                    if (SessionPtr->RegisterPlayer(SessionName, *PlayerId, false))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Successfully registered a new player in the session."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Failed to register a new player in the session."));
                    }
                }
            }
        }
    }
}
void ALobbyGameMode::Logout(AController* Exiting)
{
    if (!Exiting)
    {
        Super::Logout(Exiting);
        return;
    }

    APlayerState* ExitingPlayerState = Exiting->GetPlayerState<APlayerState>();
    if (ExitingPlayerState)
    {
        UEOSGameInstance* GI = GetGameInstance<UEOSGameInstance>();
        IOnlineSessionPtr SessionPtr = GI ? GI->GetSessionInterface() : nullptr;

        if (GI && SessionPtr.IsValid())
        {
            // Retrieve the unique ID of the player who is leaving.
            const FUniqueNetIdRepl& PlayerId = ExitingPlayerState->GetUniqueId();
            FName SessionName = GI->GetCurrentSessionName();
            
            if (PlayerId.IsValid() && SessionName != NAME_None)
            {
                UE_LOG(LogTemp, Log, TEXT("Server Logout: Unregistering player from session '%s'."), 
                       *SessionName.ToString());

                // Removes the player from the session roster in the online subsystem.
                if (SessionPtr->UnregisterPlayer(SessionName, *PlayerId))
                {
                    UE_LOG(LogTemp, Log, TEXT("Server: Successfully unregistered player."));
    
                    
                    FNamedOnlineSession* NamedSession = SessionPtr->GetNamedSession(SessionName);
                    if (NamedSession)
                    {
                        FOnlineSessionSettings& UpdatedSettings = NamedSession->SessionSettings;
                        SessionPtr->UpdateSession(SessionName, UpdatedSettings, true);
                        
                        int32 NumPlayers = NamedSession->RegisteredPlayers.Num();
                        UE_LOG(LogTemp, Log, TEXT("Server: Session updated. Remaining players: %d"), NumPlayers);
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Server: Failed to unregister player."));
                }
            }
        }
    }

    Super::Logout(Exiting);
}