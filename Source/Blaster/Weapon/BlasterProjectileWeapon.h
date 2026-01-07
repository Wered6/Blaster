// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterWeaponBase.h"
#include "BlasterProjectileWeapon.generated.h"

class ABlasterProjectile;

UCLASS()
class BLASTER_API ABlasterProjectileWeapon : public ABlasterWeaponBase
{
	GENERATED_BODY()

public:
	ABlasterProjectileWeapon();

	virtual void Fire(const FVector& HitTargetLocation) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Weapon Properties")
	TSubclassOf<ABlasterProjectile> ProjectileClass;
};
