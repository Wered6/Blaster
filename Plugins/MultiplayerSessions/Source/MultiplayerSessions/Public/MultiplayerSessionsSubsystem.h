// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/*
 * Declaring our own custom delegates for the Menu class to bind callbacks to
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateSessionCompletedSignature, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsCompletedSignature, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionCompletedSignature, EOnJoinSessionCompleteResult::Type Result)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroySessionCompletedSignature, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartSessionCompletedSignature, bool, bWasSuccessful);

class IOnlineSession;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	/*
	 * To handle session functionality. The Menu class will call these
	 */
	void CreateSession(const int32 NumPublicConnections, const FString& MatchType);
	void FindSessions(const int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionSearchResult);
	void DestroySession();
	void StartSession();

	/*
	 * Our own custom delegates for the Menu class to bind callbacks to
	 */
	FOnCreateSessionCompletedSignature OnCreateSessionCompletedDelegate;
	FOnFindSessionsCompletedSignature OnFindSessionsCompletedDelegate;
	FOnJoinSessionCompletedSignature OnJoinSessionCompletedDelegate;
	FOnDestroySessionCompletedSignature OnDestroySessionCompletedDelegate;
	FOnStartSessionCompletedSignature OnStartSessionCompletedDelegate;

protected:
	/*
	 * Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	 * These don't need to be called outside this class.
	 */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	// IOnlineSessionPtr SessionInterface; // Stephen uses this
	TWeakPtr<IOnlineSession> SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	/*
	 * To add to the Online Session Interface delegate list.
	 * We'll bind our MultiplayerSessionsSubsystem internal callbacks to these.
	 */
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
	
	bool bCreateSessionOnDestroy{false};
	int32 LastNumPublicConnections;
	FString LastMatchType;
};
