// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnPawnInitialized(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);
	
	void AddToScore(const float ScoreAmount);
	virtual void OnRep_Score() override;

private:
	TWeakObjectPtr<ABlasterCharacter> BlasterCharacter;
	TWeakObjectPtr<ABlasterPlayerController> BlasterPlayerController;
};
