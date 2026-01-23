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
	void SetHealthText(const FString& HealthString) const;
	
	void SetScoreText(const float Score) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HealthText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreAmountText;
};
