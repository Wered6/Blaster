// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Travel")
	int32 NumberOfPlayers{2};
};
