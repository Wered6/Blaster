// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlasterCombatComponent.generated.h"


class ABlasterCharacter;
class ABlasterWeaponBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API UBlasterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlasterCombatComponent();

	friend ABlasterCharacter;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(ABlasterWeaponBase* Weapon);

private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> BlasterCharacter;
	UPROPERTY(Replicated)
	TObjectPtr<ABlasterWeaponBase> EquippedWeapon;
};
