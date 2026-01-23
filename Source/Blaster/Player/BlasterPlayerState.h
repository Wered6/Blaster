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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPawnInitialized(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	void AddToScore(const float ScoreAmount);
	virtual void OnRep_Score() override;

	void AddToDefeats(const int32 DefeatsAmount);
	UFUNCTION()
	virtual void OnRep_Defeats();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Defeats)
	int32 Defeats;

	TWeakObjectPtr<ABlasterCharacter> BlasterCharacter;
	TWeakObjectPtr<ABlasterPlayerController> BlasterPlayerController;
};
