// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterCharacterOverlay.h"
#include "Blaster/HUD/BlasterHUD.h"


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

void ABlasterPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

	BlasterHUD = GetHUD<ABlasterHUD>();
	BlasterHUD->OnCharacterOverlayInitializedDelegate.AddUObject(this, &ABlasterPlayerController::OnCharacterOverlayInitialized);
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

void ABlasterPlayerController::InitPlayerState()
{
	// Happens only on server
	Super::InitPlayerState();

	ABlasterPlayerState* BlasterPlayerState{GetPlayerState<ABlasterPlayerState>()};
	BlasterPlayerState->OnPawnSet.AddDynamic(BlasterPlayerState, &ABlasterPlayerState::OnPawnInitialized);
}

void ABlasterPlayerController::SetHUDHealth(const float Health, const float MaxHealth) const
{
	const UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	const float HealthPercent{Health / MaxHealth};
	BlasterCharacterOverlayRaw->SetHealthBarPercent(HealthPercent);

	const FString HealthText{FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth))};
	BlasterCharacterOverlayRaw->SetHealthText(HealthText);
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

void ABlasterPlayerController::ShowEliminatedInfo(const float PlayRate) const
{
	UBlasterCharacterOverlay* BlasterCharacterOverlayRaw{BlasterCharacterOverlay.Get()};
	if (!ensureAlways(BlasterCharacterOverlayRaw))
	{
		return;
	}

	BlasterCharacterOverlayRaw->ShowEliminatedText(PlayRate);
}
