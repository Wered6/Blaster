// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class ABlasterCharacter;
class UBlasterCharacterOverlay;
class ABlasterHUD;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Sync with server clock as soon as possible
	virtual void ReceivedPlayer() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;

	virtual void InitPlayerState() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

private:
	TWeakObjectPtr<ABlasterCharacter> BlasterCharacter;
	TWeakObjectPtr<ABlasterHUD> BlasterHUD;
	TWeakObjectPtr<UBlasterCharacterOverlay> BlasterCharacterOverlay;

#pragma region Match

public:
	void SetMatchState(const FName State);

private:
	UFUNCTION(Server, Reliable)
	void Server_CheckMatchState();
	UFUNCTION(Client, Reliable)
	void Client_JoinMidGame(const FName InMatchState, const float InWarmupTime, const float InMatchTime);

	void OnMatchStateSet() const;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	float MatchTime{0.f};
	float WarmupTime{0.f};
	uint32 Countdown{0};

#pragma endregion

#pragma region Time

public:
	// Synced with server world clock
	virtual float GetServerTime();

private:
	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(const float TimeOfClientRequest);

	// Reports the current server time to the client in response to Server_RequestServerTime
	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(const float TimeOfClientRequest, const float TimeServerReceivedClientRequest);

	void CheckTimeSync(float DeltaSeconds);

	// Difference between client and server time
	float ClientServerDelta{0.f};

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Time")
	float TimeSyncFrequency{5.f};

	float TimeSyncRunningTime{0.f};

#pragma endregion

#pragma region HUD

public:
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;

	void SetHUDHealth(const float Health, const float MaxHealth) const;

	void SetHUDScore(const float Score) const;

	void SetHUDDefeats(const int32 Defeats) const;

	void ShowHUDEliminatedInfo(const float PlayRate) const;

	void SetHUDWeaponAmmo(const int32 AmmoAmount) const;
	void SetHUDCarriedAmmo(const int32 AmmoAmount) const;

private:
	void OnCharacterOverlayInitialized();

	void SetHUDMatchCountdown(const float CountdownTime) const;
	
	void SetHUDAnnouncementCountdown(const float CountdownTime) const;

	void SetHUDTime();

	bool bCharacterOverlayValid{false};

#pragma endregion
};
