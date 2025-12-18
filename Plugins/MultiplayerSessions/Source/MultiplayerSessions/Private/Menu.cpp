// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"

void UMenu::MenuSetup(const int32 InNumPublicConnections, FString InMatchType, const FString InLobbyPath)
{
	NumPublicConnections = InNumPublicConnections;
	MatchType = InMatchType;
	LobbyPath = FString::Printf(TEXT("%s?listen"), *InLobbyPath);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	const UWorld* World{GetWorld()};
	if (!ensure(World))
	{
		return;
	}
	APlayerController* PlayerController{World->GetFirstPlayerController()};
	if (!ensure(PlayerController))
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	InputModeUIOnly.SetWidgetToFocus(TakeWidget());
	InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeUIOnly);
	PlayerController->SetShowMouseCursor(true);

	const UGameInstance* GameInstance{GetGameInstance()};
	if (!ensure(GameInstance))
	{
		return;
	}

	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (!ensure(MultiplayerSessionsSubsystem))
	{
		return;
	}

	MultiplayerSessionsSubsystem->OnCreateSessionCompletedDelegate.AddDynamic(this, &ThisClass::OnCreateSessionCompleted);
	MultiplayerSessionsSubsystem->OnFindSessionsCompletedDelegate.AddUObject(this, &ThisClass::OnFindSessionsCompleted);
	MultiplayerSessionsSubsystem->OnJoinSessionCompletedDelegate.AddUObject(this, &ThisClass::OnJoinSessionCompleted);
	MultiplayerSessionsSubsystem->OnDestroySessionCompletedDelegate.AddDynamic(this, &ThisClass::OnDestroySessionCompleted);
	MultiplayerSessionsSubsystem->OnStartSessionCompletedDelegate.AddDynamic(this, &ThisClass::OnStartSessionCompleted);
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (!ensure(HostButton))
	{
		return false;
	}
	if (!ensure(JoinButton))
	{
		return false;
	}

	HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClicked);
	JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConst
void UMenu::OnCreateSessionCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session created successfully!"))
			);
		}

		UWorld* World{GetWorld()};
		if (!ensure(World))
		{
			return;
		}
		World->ServerTravel(LobbyPath);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!"))
			);
		}
		HostButton->SetIsEnabled(true);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConst
void UMenu::OnFindSessionsCompleted(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!ensure(MultiplayerSessionsSubsystem))
	{
		return;
	}

	for (FOnlineSessionSearchResult Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConst
void UMenu::OnJoinSessionCompleted(EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!ensure(Subsystem))
	{
		return;
	}

	const IOnlineSessionPtr SessionInterface{Subsystem->GetSessionInterface()};
	if (SessionInterface.IsValid())
	{
		FString Address;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

		APlayerController* PlayerController{GetGameInstance()->GetFirstLocalPlayerController()};
		if (!ensure(PlayerController))
		{
			return;
		}

		PlayerController->ClientTravel(Address, TRAVEL_Absolute);
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UMenu::OnDestroySessionCompleted(bool bWasSuccessful)
{
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UMenu::OnStartSessionCompleted(bool bWasSuccessful)
{
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();

	const UWorld* World{GetWorld()};
	if (!ensure(World))
	{
		return;
	}

	APlayerController* PlayerController{World->GetFirstPlayerController()};
	if (PlayerController)
	{
		const FInputModeGameOnly InputModeGameOnly;
		PlayerController->SetInputMode(InputModeGameOnly);
		PlayerController->SetShowMouseCursor(false);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMenu::OnHostButtonClicked()
{
	HostButton->SetIsEnabled(false);

	if (!ensure(MultiplayerSessionsSubsystem))
	{
		return;
	}

	MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMenu::OnJoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);

	if (!ensure(MultiplayerSessionsSubsystem))
	{
		return;
	}

	MultiplayerSessionsSubsystem->FindSessions(10000);
}
