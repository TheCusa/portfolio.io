// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "EOSGameInstance.generated.h"

class UInvitePopup;
class IOnlineLobby;
class FOnlineLobbyId;

namespace EOnJoinSessionCompleteResult
{
    enum Type;
}

// --- Delegates ---
DECLARE_MULTICAST_DELEGATE(FOnLoginSuccess);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginFailed, const FString& /*Error*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCustomSessionInviteReceived, const FUniqueNetId& /*FromId*/, const FOnlineSessionSearchResult& /*InviteResult*/);

UCLASS()
class COOPGAME_API UEOSGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // --- Delegates ---
    FOnLoginSuccess OnLoginSuccess;
    FOnLoginFailed OnLoginFailed;
    FOnCustomSessionInviteReceived OnSessionInviteReceived;

    // --- Main Functions ---
    UFUNCTION(BlueprintCallable, Category = "EOS")
    void Login();

    UFUNCTION(BlueprintCallable, Category = "EOS")
    void CreateSession(bool bIsPublic = false);

    UFUNCTION(BlueprintCallable, Category = "EOS")
    FString GetNickname();

    void LoadGameLevel();

    // --- UI & Invites ---
    UFUNCTION(BlueprintCallable, Category = "EOS|UI")
    void ShowFriendsUI();

    UFUNCTION(BlueprintCallable, Category = "EOS|Invites")
    void AcceptLastReceivedInvite();

    // --- State Functions ---
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS|Session")
    bool IsInSession() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS|Session")
    bool IsSessionHost() const;

    UFUNCTION(BlueprintCallable, Category = "EOS|Session")
    void LeaveSession();

    /** Leaves the current session and travels back to the main menu level. */
    UFUNCTION(BlueprintCallable, Category = "EOS|Session")
    void ReturnToMainMenu();

    /** Checks if the user is currently logged into the online subsystem. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS|Login")
    bool IsLoggedIn() const;
    
    FString GetSessionNameFromSearchResult(const FOnlineSessionSearchResult& SearchResult) const;
    FORCEINLINE FName GetCurrentSessionName() const { return CurrentSessionName; };
    FORCEINLINE IOnlineSessionPtr GetSessionInterface() const { return SessionPtr; }
    
protected:
    virtual void Init() override;
    virtual void Shutdown() override;

private:
    // --- Online Subsystem Pointers ---
    TSharedPtr<const FUniqueNetId> LoggedInUserId;
    IOnlineSubsystem* OnlineSubsystem;
    TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> IdentityPtr;
    TSharedPtr<IOnlineSession, ESPMode::ThreadSafe> SessionPtr;
    TSharedPtr<IOnlineExternalUI, ESPMode::ThreadSafe> ExternalUIPtr;
    
    // --- Invite Management ---
    TSharedPtr<FOnlineSessionSearchResult> LastInviteResult;

    // --- Game Levels ---
    
    UPROPERTY(EditDefaultsOnly, Category = "Levels")
    TSoftObjectPtr<UWorld> LoginLevel;

    UPROPERTY(EditDefaultsOnly, Category = "Levels")
    TSoftObjectPtr<UWorld> LobbyLevel;
    
    UPROPERTY(EditDefaultsOnly, Category = "Levels")
    TSoftObjectPtr<UWorld> GameLevel;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UInvitePopup> InvitePopupClass;

    // --- Session State ---
    const FName SESSION_SETTINGS_KEY_NAME{ "SessionName" };
    FName CurrentSessionName;
    bool bIsInSession = false;
    bool bIsHost = false;

    // --- Delegate Handles ---
    FDelegateHandle OnSessionInviteReceivedHandle;
    FDelegateHandle OnSessionUserInviteAcceptedHandle;
    
    
    // --- Online Subsystem Callbacks ---
    void OnLoginCompleted(int ControllerId, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
    void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
    void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
    void OnSessionInviteReceivedCallback(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult);
    void OnSessionUserInviteAccepted(bool bWasSuccessful, int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);
    void OnUpdateSessionCompleted(FName SessionName, bool bWasSuccessful);
    
    // --- Private Helper Functions ---
    void AcceptSessionInvite(int32 LocalUserNum, const FOnlineSessionSearchResult& InviteResult);
    void TravelToLevel(TSoftObjectPtr<UWorld> Level, bool bAsListenServer);
    FString GenerateRandomSessionName(int32 Length = 16) const;
};