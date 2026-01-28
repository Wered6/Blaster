// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterCharacterOverlay.h"
#include "Blaster/HUD/BlasterHUD.h"


void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!bCharacterOverlayValid)
	{
		return;
	}

	if (IsLocalController())
	{
		const ABlasterCharacter* BlasterCharacter{Cast<ABlasterCharacter>(InPawn)};
		// Happens for local authoritative controller at respawn 
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (!bCharacterOverlayValid)
	{
		return;
	}

	const ABlasterCharacter* BlasterCharacter{GetPawn<ABlasterCharacter>()};
	// Happens for autonomous proxies at spawn and respawn 
	SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
}

void ABlasterPlayerController::InitPlayerState()
{
	// Happens only on server
	Super::InitPlayerState();

	ABlasterPlayerState* BlasterPlayerState{GetPlayerState<ABlasterPlayerState>()};
	BlasterPlayerState->OnPawnSet.AddDynamic(BlasterPlayerState, &ABlasterPlayerState::OnPawnInitialized);
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();

	CheckTimeSync(DeltaSeconds);
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::CheckTimeSync(const float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;
	if (IsLocalController() && TimeSyncRunningTime >= TimeSyncFrequency)
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::Server_RequestServerTime_Implementation(const float TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	Client_ReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::Client_ReportServerTime_Implementation(const float TimeOfClientRequest, const float TimeServerReceivedClientRequest)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	const float CurrentServerTime = TimeServerReceivedClientRequest + RoundTripTime / 2.f;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ABlasterPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

	BlasterHUD = GetHUD<ABlasterHUD>();
	BlasterHUD->OnCharacterOverlayInitializedDelegate.AddUObject(this, &ABlasterPlayerController::OnCharacterOverlayInitialized);
}

void ABlasterPlayerController::SetHUDHealth(const float Health, const float MaxHealth) const
{
	const UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->SetHealthBarPercent(Health / MaxHealth);
	BlasterCharacterOverlayRaw->SetHealthText(Health, MaxHealth);
}

void ABlasterPlayerController::SetHUDScore(const float Score) const
{
	const UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->SetScoreText(Score);
}

void ABlasterPlayerController::SetHUDDefeats(const int32 Defeats) const
{
	const UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->SetDefeatsText(Defeats);
}

void ABlasterPlayerController::ShowHUDEliminatedInfo(const float PlayRate) const
{
	UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->ShowEliminatedText(PlayRate);
}

void ABlasterPlayerController::SetHUDWeaponAmmo(const int32 AmmoAmount) const
{
	const UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->SetWeaponAmmoAmountText(AmmoAmount);
}

void ABlasterPlayerController::SetHUDCarriedAmmo(const int32 AmmoAmount) const
{
	const UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->SetCarriedAmmoAmountText(AmmoAmount);
}

void ABlasterPlayerController::OnCharacterOverlayInitialized()
{
	BlasterCharacterOverlay = BlasterHUD->GetCharacterOverlay();

	bCharacterOverlayValid = true;

	if (HasAuthority())
	{
		const ABlasterCharacter* BlasterCharacter{GetPawn<ABlasterCharacter>()};
		// Happens for local authoritative controller at spawn
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(const float CountdownTime) const
{
	const UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->SetMatchCountdownText(CountdownTime);
}

void ABlasterPlayerController::SetHUDTime()
{
	if (!bCharacterOverlayValid)
	{
		return;
	}

	const float RemainingSeconds = MatchTime - GetServerTime();
	const uint32 SecondsLeft = FMath::CeilToInt(RemainingSeconds);
	if (SecondsLeft != Countdown)
	{
		SetHUDMatchCountdown(RemainingSeconds);
	}

	Countdown = SecondsLeft;
}
