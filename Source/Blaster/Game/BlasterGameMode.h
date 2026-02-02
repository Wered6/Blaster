// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
}

class ABlasterPlayerController;
class ABlasterCharacter;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();

protected:
	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;

public:
	void PlayerEliminated(ABlasterCharacter* EliminatedCharacter,
	                      ABlasterPlayerController* VictimController,
	                      ABlasterPlayerController* AttackerController) const;

	void RequestRespawn(ACharacter* EliminatedCharacter, AController* VictimController);

	FORCEINLINE float GetWarmupTime() const
	{
		return WarmupTime;
	}

	FORCEINLINE float GetMatchTime() const
	{
		return MatchTime;
	}

	FORCEINLINE float GetCooldownTime() const
	{
		return CooldownTime;
	}

private:
	void OnWarmupTimerCompleted();
	void OnMatchTimerCompleted();
	void OnCooldownTimerCompleted();
	
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Match State")
	float WarmupTime{10.f};
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Match State")
	float MatchTime{120.f};
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Match State")
	float CooldownTime{10.f};
	
	FTimerHandle WarmupTimerHandle;
	FTimerHandle MatchTimerHandle;
	FTimerHandle CooldownTimerHandle;
};
