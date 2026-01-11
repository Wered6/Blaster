// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

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
	void PlayerEliminated(ABlasterCharacter* EliminatedCharacter,
	                      ABlasterPlayerController* VictimController,
	                      ABlasterPlayerController* AttackerController) const;
	
	void RequestRespawn(ACharacter* EliminatedCharacter, AController* VictimController);
};
