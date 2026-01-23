// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterCharacterOverlay.h"
#include "Blaster/HUD/BlasterHUD.h"


void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		const ABlasterCharacter* BlasterCharacter{Cast<ABlasterCharacter>(InPawn)};
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::SetHUDHealth(const float Health, const float MaxHealth) const
{
	const ABlasterHUD* BlasterHUD{GetHUD<ABlasterHUD>()};
	if (!BlasterHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: machine: %s, BlasterHUD is nullptr"),
		       TEXT(__FUNCTION__),
		       UE::GetPlayInEditorID() ? *FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID()) : TEXT("Server")
		)
		return;
	}
	// When player spawns for the first time BlasterCharacterOverlay is nullptr here,
	// so in ABlasterCharacter::BeginPlay() we cover HUDHealth,
	// but when player respawns there is need to do it in ABlasterPlayerController::OnPossess(APawn* InPawn) 
	const UBlasterCharacterOverlay* BlasterCharacterOverlay{BlasterHUD->GetCharacterOverlay()};
	if (!BlasterCharacterOverlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: machine: %s, BlasterCharacterOverlay is nullptr"),
		       TEXT(__FUNCTION__),
		       UE::GetPlayInEditorID() ? *FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID()) : TEXT("Server"))
		return;
	}

	const float HealthPercent{Health / MaxHealth};
	BlasterCharacterOverlay->SetHealthBarPercent(HealthPercent);

	const FString HealthText{FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth))};
	BlasterCharacterOverlay->SetHealthText(HealthText);
}

void ABlasterPlayerController::SetHUDScore(const float Score) const
{
	const ABlasterHUD* BlasterHUD{GetHUD<ABlasterHUD>()};
	if (!BlasterHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: machine: %s, BlasterHUD is nullptr"),
		       TEXT(__FUNCTION__),
		       UE::GetPlayInEditorID() ? *FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID()) : TEXT("Server")
		)
		return;
	}
	const UBlasterCharacterOverlay* BlasterCharacterOverlay{BlasterHUD->GetCharacterOverlay()};
	if (!BlasterCharacterOverlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: machine: %s, BlasterCharacterOverlay is nullptr"),
		       TEXT(__FUNCTION__),
		       UE::GetPlayInEditorID() ? *FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID()) : TEXT("Server"))
		return;
	}

	BlasterCharacterOverlay->SetScoreText(Score);
}

void ABlasterPlayerController::SetHUDDefeats(const int32 Defeats) const
{
	const ABlasterHUD* BlasterHUD{GetHUD<ABlasterHUD>()};
	if (!BlasterHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: machine: %s, BlasterHUD is nullptr"),
		       TEXT(__FUNCTION__),
		       UE::GetPlayInEditorID() ? *FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID()) : TEXT("Server")
		)
		return;
	}
	const UBlasterCharacterOverlay* BlasterCharacterOverlay{BlasterHUD->GetCharacterOverlay()};
	if (!BlasterCharacterOverlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: machine: %s, BlasterCharacterOverlay is nullptr"),
		       TEXT(__FUNCTION__),
		       UE::GetPlayInEditorID() ? *FString::Printf(TEXT("Client %d"), UE::GetPlayInEditorID()) : TEXT("Server"))
		return;
	}

	BlasterCharacterOverlay->SetDefeatsText(Defeats);
}

void ABlasterPlayerController::InitPlayerState()
{
	// Happens only on server
	Super::InitPlayerState();

	ABlasterPlayerState* BlasterPlayerState{GetPlayerState<ABlasterPlayerState>()};
	BlasterPlayerState->OnPawnSet.AddDynamic(BlasterPlayerState, &ABlasterPlayerState::OnPawnInitialized);
}
