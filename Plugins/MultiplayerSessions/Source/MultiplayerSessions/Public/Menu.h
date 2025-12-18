// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(const int32 InNumPublicConnections = 4,
	               FString InMatchType = FString(TEXT("FreeForAll")),
	               const FString InLobbyPath = FString(TEXT("/Game/ThirdPerson/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	/*
	 * Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	 */
	UFUNCTION()
	void OnCreateSessionCompleted(bool bWasSuccessful);
	void OnFindSessionsCompleted(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSessionCompleted(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySessionCompleted(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSessionCompleted(bool bWasSuccessful);

private:
	void MenuTearDown();

	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FString LobbyPath{};

	UFUNCTION()
	void OnHostButtonClicked();
	UFUNCTION()
	void OnJoinButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> JoinButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> HostButton;

	/*
	 * The subsystem designed to handle all online session functionality
	 */
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
};
