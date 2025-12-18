// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	// IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(); // Stephen uses this
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(UObject::GetWorld());
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(const int32 NumPublicConnections, const FString& MatchType)
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (!ensure(Session.IsValid()))
	{
		return;
	}

	const FNamedOnlineSession* ExistingSession{Session->GetNamedSession(NAME_GameSession)};
	if (ExistingSession)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
	}

	// Store the delegate in a FDelegateHandle se we can later remove it from the delegate list
	CreateSessionCompleteDelegateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL";
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->BuildUniqueId = 1;

	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer{GetWorld()->GetFirstLocalPlayerFromController()};
	if (!Session->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// Broadcast our own custom delegate
		OnCreateSessionCompletedDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(const int32 MaxSearchResults)
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (!ensure(Session.IsValid()))
	{
		return;
	}

	FindSessionsCompleteDelegateHandle = Session->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL";
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	// LastSessionSearch->QuerySettings.Set(FName("MatchType"), FString("FreeForAll"), EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer{GetWorld()->GetFirstLocalPlayerFromController()};
	if (!Session->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		Session->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		// Broadcast our own custom delegate
		OnFindSessionsCompletedDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionSearchResult)
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (!Session.IsValid())
	{
		OnJoinSessionCompletedDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer{GetWorld()->GetFirstLocalPlayerFromController()};
	if (!Session->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearchResult))
	{
		Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		OnJoinSessionCompletedDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (!Session.IsValid())
	{
		OnDestroySessionCompletedDelegate.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = Session->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!Session->DestroySession(NAME_GameSession))
	{
		Session->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		OnDestroySessionCompletedDelegate.Broadcast(false);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UMultiplayerSessionsSubsystem::StartSession()
{
}

// ReSharper disable once CppParameterMayBeConst
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (Session.IsValid())
	{
		Session->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	OnCreateSessionCompletedDelegate.Broadcast(bWasSuccessful);
}

// ReSharper disable once CppParameterMayBeConst
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (Session.IsValid())
	{
		Session->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		OnFindSessionsCompletedDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	OnFindSessionsCompletedDelegate.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (Session.IsValid())
	{
		Session->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	OnJoinSessionCompletedDelegate.Broadcast(Result);
}

// ReSharper disable once CppParameterMayBeConst
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	const TSharedPtr<IOnlineSession> Session{SessionInterface.Pin()};
	if (Session.IsValid())
	{
		Session->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	OnDestroySessionCompletedDelegate.Broadcast(bWasSuccessful);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
