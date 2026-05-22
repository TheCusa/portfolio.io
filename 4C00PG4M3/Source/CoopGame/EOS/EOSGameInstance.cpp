// Fill out your copyright notice in the Description page of Project Settings.

#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "CoopGame/Widgets/InvitePopup.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/SoftObjectPath.h"

void UEOSGameInstance::Init()
{
    Super::Init();
    
    OnlineSubsystem = Online::GetSubsystem(GetWorld());
    if (!OnlineSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem not found!"));
        return;
    }

    IdentityPtr = OnlineSubsystem->GetIdentityInterface();
    if (IdentityPtr.IsValid())
    {
        IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginCompleted);
    }
    
    SessionPtr = OnlineSubsystem->GetSessionInterface();
    if (SessionPtr.IsValid())
    {
        SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionCompleted);
        SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionCompleted);
        SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionCompleted);
        SessionPtr->OnUpdateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnUpdateSessionCompleted);
        
        OnSessionInviteReceivedHandle = SessionPtr->AddOnSessionInviteReceivedDelegate_Handle(
            FOnSessionInviteReceivedDelegate::CreateUObject(this, &UEOSGameInstance::OnSessionInviteReceivedCallback)
        );
        OnSessionUserInviteAcceptedHandle = SessionPtr->AddOnSessionUserInviteAcceptedDelegate_Handle(
            FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UEOSGameInstance::OnSessionUserInviteAccepted)
        );
    }

    ExternalUIPtr = OnlineSubsystem->GetExternalUIInterface();
}

void UEOSGameInstance::Shutdown()
{
    if (SessionPtr.IsValid())
    {
        SessionPtr->ClearOnSessionInviteReceivedDelegate_Handle(OnSessionInviteReceivedHandle);
        SessionPtr->ClearOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedHandle);
    }
    Super::Shutdown();
}

void UEOSGameInstance::Login()
{
    if (IdentityPtr.IsValid())
    {
        if (IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn)
        {
            UE_LOG(LogTemp, Warning, TEXT("User already logged in. Broadcasting success."));
            OnLoginSuccess.Broadcast();
            return;
        }
        FOnlineAccountCredentials Credentials("AccountPortal", "", "");
        IdentityPtr->Login(0, Credentials);
    }
}

void UEOSGameInstance::CreateSession(bool bIsPublic)
{
    if (!SessionPtr.IsValid() || !LoggedInUserId.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot create session: User not logged in or session interface invalid."));
        return;
    }
    
    FName SessionName = FName(*GenerateRandomSessionName());
    UE_LOG(LogTemp, Log, TEXT("Creating session with random name: %s"), *SessionName.ToString());
    
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowInvites = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bIsLANMatch = false;
    SessionSettings.bShouldAdvertise = bIsPublic;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.NumPublicConnections = 2;

    SessionSettings.Set(SESSION_SETTINGS_KEY_NAME, SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
    
    CurrentSessionName = SessionName;
    SessionPtr->CreateSession(*LoggedInUserId, SessionName, SessionSettings);
}

FString UEOSGameInstance::GetNickname()
{
    if (IdentityPtr.IsValid() && LoggedInUserId.IsValid())
    {
        return IdentityPtr->GetPlayerNickname(*LoggedInUserId);
    }
    return TEXT("Unknown");
}

void UEOSGameInstance::LoadGameLevel()
{
    TravelToLevel(GameLevel, true);
}

void UEOSGameInstance::ShowFriendsUI()
{
    if (!ExternalUIPtr.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot show Friends UI: External UI Interface is not valid."));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("Attempting to show Friends UI via EOS Overlay."));
    ExternalUIPtr->ShowFriendsUI(0);
}

void UEOSGameInstance::AcceptLastReceivedInvite()
{
    if (LastInviteResult.IsValid())
    {
        AcceptSessionInvite(0, *LastInviteResult);
        LastInviteResult.Reset();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AcceptLastReceivedInvite called, but there is no pending invite."));
    }
}

bool UEOSGameInstance::IsInSession() const { return bIsInSession; }
bool UEOSGameInstance::IsSessionHost() const { return bIsHost; }

void UEOSGameInstance::LeaveSession()
{
    if (SessionPtr.IsValid() && IsInSession())
    {
        UE_LOG(LogTemp, Log, TEXT("Destroying session: %s"), *CurrentSessionName.ToString());
        SessionPtr->DestroySession(CurrentSessionName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LeaveSession called but not in a valid session or session pointer invalid."));
    }
}

void UEOSGameInstance::ReturnToMainMenu()
{
    APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
    if (!PC) return;
    
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = false;
    
    if (bIsHost)
    {
        UE_LOG(LogTemp, Log, TEXT("Host is returning to main menu. Destroying session."));
        LeaveSession();
        
        FString LevelPath = LoginLevel.ToSoftObjectPath().GetAssetPathString();
        FString MapPackagePath;
        FString AssetName;
        LevelPath.Split(TEXT("."), &MapPackagePath, &AssetName);

        if (MapPackagePath.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse map path for MainMenuLevel. Path was: %s"), *LevelPath);
            return;
        }
        UE_LOG(LogTemp, Log, TEXT("Host traveling to Main Menu: %s"), *MapPackagePath);
        PC->ClientTravel(MapPackagePath, ETravelType::TRAVEL_Absolute);
    }
    else if (bIsInSession)
    {
        UE_LOG(LogTemp, Log, TEXT("Client leaving session and returning to main menu."));
        LeaveSession();
        
        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, PC]()
        {
            UE_LOG(LogTemp, Log, TEXT("Client disconnecting after cleanup delay"));
            PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Leaving session.")));
        }, 0.1f, false);
    }
    else
    {
        FString LevelPath = LoginLevel.ToSoftObjectPath().GetAssetPathString();
        FString MapPackagePath;
        FString AssetName;
        LevelPath.Split(TEXT("."), &MapPackagePath, &AssetName);

        if (MapPackagePath.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse map path for MainMenuLevel. Path was: %s"), *LevelPath);
            return;
        }
        UE_LOG(LogTemp, Log, TEXT("Not in session, traveling to Main Menu: %s"), *MapPackagePath);
        PC->ClientTravel(MapPackagePath, ETravelType::TRAVEL_Absolute);
    }
}

bool UEOSGameInstance::IsLoggedIn() const
{
    if (IdentityPtr.IsValid())
    {
        return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
    }
    return false;
}

FString UEOSGameInstance::GetSessionNameFromSearchResult(const FOnlineSessionSearchResult& SearchResult) const
{
    FString OutVal;
    SearchResult.Session.SessionSettings.Get(SESSION_SETTINGS_KEY_NAME, OutVal);
    return OutVal;
}

void UEOSGameInstance::OnLoginCompleted(int ControllerId, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Login successful for user: %s"), *UserId.ToString());
        LoggedInUserId = UserId.AsShared();
        OnLoginSuccess.Broadcast();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Login failed: %s"), *Error);
        LoggedInUserId.Reset();
        OnLoginFailed.Broadcast(Error);
    }
}

void UEOSGameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session '%s' created successfully."), *SessionName.ToString());
        bIsInSession = true;
        bIsHost = true;
        TravelToLevel(LobbyLevel, true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session '%s'."), *SessionName.ToString());
        bIsInSession = false;
        bIsHost = false;
    }
}

void UEOSGameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        FString TravelUrl;
        if (SessionPtr->GetResolvedConnectString(SessionName, TravelUrl))
        {
            UE_LOG(LogTemp, Log, TEXT("Joining session '%s'. Client traveling to: %s"), *SessionName.ToString(), *TravelUrl);
            bIsInSession = true;
            bIsHost = false;

            CurrentSessionName = SessionName;
            
            APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
            if (PC)
            {
                PC->ClientTravel(TravelUrl, ETravelType::TRAVEL_Absolute);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get connect string for session '%s'."), *SessionName.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to join session '%s'. Result: %d"), *SessionName.ToString(), static_cast<int32>(Result));
        bIsInSession = false;
        bIsHost = false;
    }
}

void UEOSGameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("OnDestroySessionCompleted: Session '%s' destroyed. Success: %d, WasHost: %d"), 
           *SessionName.ToString(), bWasSuccessful, bIsHost);
    
    bIsInSession = false;
    bool WasHost = bIsHost;
    bIsHost = false;
    CurrentSessionName = NAME_None;

    if (WasHost)
    {
        UE_LOG(LogTemp, Log, TEXT("Host session fully destroyed"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Client session fully destroyed"));
    }
}

void UEOSGameInstance::OnSessionInviteReceivedCallback(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult)
{
    FString FromPlayerName = TEXT("Unknown");
    if (IdentityPtr.IsValid())
    {
        FromPlayerName = IdentityPtr->GetPlayerNickname(FromId);
    }
    UE_LOG(LogTemp, Log, TEXT("Session invite received from: %s"), *FromPlayerName);
    LastInviteResult = MakeShared<FOnlineSessionSearchResult>(InviteResult);
    
    if (!InvitePopupClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("InvitePopupClass is not set in the EOSGameInstance. Cannot show invite popup."));
        return;
    }
    UInvitePopup* InvitePopup = CreateWidget<UInvitePopup>(this, InvitePopupClass);
    if (InvitePopup)
    {
        InvitePopup->Setup(FromPlayerName); 
        InvitePopup->AddToViewport();
    }
    OnSessionInviteReceived.Broadcast(FromId, InviteResult);
}

void UEOSGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("User accepted invite, joining session..."));
        AcceptSessionInvite(ControllerId, InviteResult);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to process accepted invite."));
    }
}

void UEOSGameInstance::OnUpdateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session '%s' updated successfully."), *SessionName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to update session '%s'."), *SessionName.ToString());
    }
}

void UEOSGameInstance::AcceptSessionInvite(int32 LocalUserNum, const FOnlineSessionSearchResult& InviteResult)
{
    if (!SessionPtr.IsValid()) return;

    if (IsInSession())
    {
        UE_LOG(LogTemp, Warning, TEXT("Leaving current session before accepting invite."));
        LeaveSession();
    }

    const FName SessionNameToJoin = FName(*GetSessionNameFromSearchResult(InviteResult));
    if (!SessionNameToJoin.IsNone())
    {
        SessionPtr->JoinSession(LocalUserNum, SessionNameToJoin, InviteResult);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not get session name from invite result."));
    }
}

void UEOSGameInstance::TravelToLevel(TSoftObjectPtr<UWorld> Level, bool bAsListenServer)
{
    if (UWorld* World = GetWorld())
    {
        FString FullAssetPath = Level.ToSoftObjectPath().GetAssetPathString();
        FString MapPackagePath;
        FString AssetName; 
        FullAssetPath.Split(TEXT("."), &MapPackagePath, &AssetName);
        UE_LOG(LogTemp, Log, TEXT("Traveling to level with cleaned path: %s"), *MapPackagePath);

        FString Options = bAsListenServer ? TEXT("?listen") : TEXT("");
        
        // Use the cleaned path for ServerTravel
        World->ServerTravel(MapPackagePath + Options);
    }
}

FString UEOSGameInstance::GenerateRandomSessionName(int32 Length) const
{
    const FString AlphanumericChars = TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    FString RandomString;
    RandomString.Reserve(Length);
    for (int32 i = 0; i < Length; ++i)
    {
        int32 RandomIndex = FMath::RandRange(0, AlphanumericChars.Len() - 1);
        RandomString.AppendChar(AlphanumericChars[RandomIndex]);
    }
    return RandomString;
}