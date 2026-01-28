// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlasterCharacterOverlay.generated.h"

class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(const float Percent) const;
	void SetHealthText(float Health, float MaxHealth) const;

	void SetScoreText(const float Score) const;

	void SetDefeatsText(const int32 Defeats) const;

	void ShowEliminatedText(float PlayRate);

	void SetWeaponAmmoAmountText(const int32 AmmoAmount) const;
	void SetCarriedAmmoAmountText(const int32 AmmoAmount) const;
	
	void SetMatchCountdownText(const float CountdownTime) const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreAmountText;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DefeatsAmountText;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EliminatedInfoAnim;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WeaponAmmoAmountText;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CarriedAmmoAmountText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MatchCountdownText;
};
