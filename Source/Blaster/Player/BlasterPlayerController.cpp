// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterCharacterOverlay.h"
#include "Blaster/HUD/BlasterHUD.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}

void ABlasterPlayerController::SetHUDHealth(const float Health, const float MaxHealth) const
{
	const float HealthPercent{Health / MaxHealth};
	BlasterHUD->GetCharacterOverlay()->SetHealthBarPercent(HealthPercent);

	const FString HealthText{FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth))};
	BlasterHUD->GetCharacterOverlay()->SetHealthText(HealthText);
}
