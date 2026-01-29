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

void UBlasterCharacterOverlay::SetHealthText(const float Health, const float MaxHealth) const
{
	if (!ensure(HealthText))
	{
		return;
	}

	HealthText->SetText(FText::Format(FText::FromString(TEXT("{0}/{1}")),
	                                  FText::AsNumber(FMath::CeilToInt(Health)),
	                                  FText::AsNumber(FMath::CeilToInt(MaxHealth))));
}

void UBlasterCharacterOverlay::SetScoreText(const float Score) const
{
	if (!ensure(ScoreAmountText))
	{
		return;
	}

	ScoreAmountText->SetText(FText::AsNumber(Score));
}

void UBlasterCharacterOverlay::SetDefeatsText(const int32 Defeats) const
{
	if (!ensure(DefeatsAmountText))
	{
		return;
	}

	DefeatsAmountText->SetText(FText::AsNumber(Defeats));
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

	WeaponAmmoAmountText->SetText(FText::AsNumber(AmmoAmount));
}

void UBlasterCharacterOverlay::SetCarriedAmmoAmountText(const int32 AmmoAmount) const
{
	if (!ensure(CarriedAmmoAmountText))
	{
		return;
	}

	CarriedAmmoAmountText->SetText(FText::AsNumber(AmmoAmount));
}

void UBlasterCharacterOverlay::SetMatchCountdownText(const float CountdownTime) const
{
	if (!ensure(MatchCountdownText))
	{
		return;
	}

	if (CountdownTime < 0.f)
	{
		MatchCountdownText->SetText(FText());
		return;
	}

	const int32 TotalSeconds{FMath::FloorToInt(CountdownTime)};
	const int32 Minutes{TotalSeconds / 60};
	const int32 Seconds{TotalSeconds % 60};

	FNumberFormattingOptions Options;
	Options.SetMinimumIntegralDigits(2);

	MatchCountdownText->SetText(FText::Format(FText::FromString(TEXT("{0}:{1}")),
	                                          FText::AsNumber(Minutes, &Options),
	                                          FText::AsNumber(Seconds, &Options)));
}
