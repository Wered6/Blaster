// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Game/BlasterGameMode.h"
#include "Blaster/HUD/BlasterAnnouncementWidget.h"
#include "Blaster/HUD/BlasterCharacterOverlay.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"


void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState)
}

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

	BlasterCharacter = Cast<ABlasterCharacter>(InPawn);

	if (!bCharacterOverlayValid)
	{
		return;
	}

	if (IsLocalController())
	{
		const ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};
		// Happens for local authoritative controller at respawn 
		SetHUDHealth(BlasterCharacterRaw->GetHealth(), BlasterCharacterRaw->GetMaxHealth());
	}
}

void ABlasterPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	BlasterCharacter = GetPawn<ABlasterCharacter>();

	if (!bCharacterOverlayValid)
	{
		return;
	}

	const ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};
	// Happens for autonomous proxies at respawn 
	SetHUDHealth(BlasterCharacterRaw->GetHealth(), BlasterCharacterRaw->GetMaxHealth());
}

void ABlasterPlayerController::InitPlayerState()
{
	// Happens only on server
	Super::InitPlayerState();

	ABlasterPlayerState* BlasterPlayerState{GetPlayerState<ABlasterPlayerState>()};
	BlasterPlayerState->OnPawnSet.AddDynamic(BlasterPlayerState, &ABlasterPlayerState::OnPawnInitialized);
}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Server_CheckMatchState();
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();

	CheckTimeSync(DeltaSeconds);
}

void ABlasterPlayerController::SetMatchState(const FName State)
{
	MatchState = State;

	OnMatchStateSet();
}

void ABlasterPlayerController::Server_CheckMatchState_Implementation()
{
	const ABlasterGameMode* GameMode{GetWorld()->GetAuthGameMode<ABlasterGameMode>()};

	MatchState = GameMode->GetMatchState();
	WarmupTime = GameMode->GetWarmupTime();
	MatchTime = GameMode->GetMatchTime();

	Client_JoinMidGame(MatchState, WarmupTime, MatchTime);
}

void ABlasterPlayerController::Client_JoinMidGame_Implementation(const FName InMatchState, const float InWarmupTime, const float InMatchTime)
{
	MatchState = InMatchState;
	WarmupTime = InWarmupTime;
	MatchTime = InMatchTime;

	if (MatchState == MatchState::WaitingToStart && IsLocalController())
	{
		BlasterHUD->AddAnnouncementWidget();
	}
}

void ABlasterPlayerController::OnMatchStateSet() const
{
	if (!IsLocalController())
	{
		return;
	}

	if (MatchState == MatchState::InProgress)
	{
		BlasterHUD->AddCharacterOverlay();
		BlasterHUD->GetAnnouncementWidget()->SetVisibility(ESlateVisibility::Hidden);

		const ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};
		// Happens for autonomous proxies at spawn
		SetHUDHealth(BlasterCharacterRaw->GetHealth(), BlasterCharacterRaw->GetMaxHealth());
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterPlayerController::OnRep_MatchState()
{
	OnMatchStateSet();
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

	const ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};
	if (HasAuthority() && BlasterCharacterRaw)
	{
		// Happens for local authoritative controller at spawn
		SetHUDHealth(BlasterCharacterRaw->GetHealth(), BlasterCharacterRaw->GetMaxHealth());
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

void ABlasterPlayerController::SetHUDAnnouncementCountdown(const float CountdownTime) const
{
	const UBlasterAnnouncementWidget* BlasterAnnouncementWidget{BlasterHUD->GetAnnouncementWidget()};
	// Not true in first call in SetHUDTime() for clients
	if (BlasterAnnouncementWidget)
	{
		BlasterAnnouncementWidget->SetWarmupTimeText(CountdownTime);
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	if (!IsLocalController())
	{
		return;
	}

	float TimeLeft{0.f};
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime();
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime();
	}

	const uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (SecondsLeft != Countdown)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	Countdown = SecondsLeft;
}
