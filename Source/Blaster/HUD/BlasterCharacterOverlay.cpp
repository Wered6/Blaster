// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBlasterCharacterOverlay::SetHealthBarPercent(const float Percent) const
{
	if (!ensure(HealthBar))
	{
		return;
	}

	HealthBar->SetPercent(Percent);
}

void UBlasterCharacterOverlay::SetHealthText(const FString& HealthString) const
{
	if (!ensure(HealthText))
	{
		return;
	}

	HealthText->SetText(FText::FromString(HealthString));
}

void UBlasterCharacterOverlay::SetScoreText(const float Score) const
{
	if (!ensure(ScoreAmountText))
	{
		return;
	}

	ScoreAmountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::FloorToInt(Score))));
}

void UBlasterCharacterOverlay::SetDefeatsText(const int32 Defeats) const
{
	if (!ensure(DefeatsAmountText))
	{
		return;
	}

	DefeatsAmountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Defeats)));
}

void UBlasterCharacterOverlay::ShowEliminatedText(const float PlayRate)
{
	if (!ensure(EliminatedInfoAnim))
	{
		return;
	}

	PlayAnimation(EliminatedInfoAnim,
	              0.f,
	              1,
	              EUMGSequencePlayMode::Forward,
	              1.f / PlayRate
	);
}

void UBlasterCharacterOverlay::SetWeaponAmmoAmountText(const int32 AmmoAmount) const
{
	if (!ensure(WeaponAmmoAmountText))
	{
		return;
	}

	WeaponAmmoAmountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), AmmoAmount)));
}
