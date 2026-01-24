// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class UBlasterCharacterOverlay;
class ABlasterHUD;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;
	
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	void OnCharacterOverlayInitialized();

	virtual void InitPlayerState() override;

	void SetHUDHealth(const float Health, const float MaxHealth) const;

	void SetHUDScore(const float Score) const;

	void SetHUDDefeats(const int32 Defeats) const;

private:
	bool bCharacterOverlayValid{false};
	
	TWeakObjectPtr<ABlasterHUD> BlasterHUD;
	TWeakObjectPtr<UBlasterCharacterOverlay> BlasterCharacterOverlay;
};
