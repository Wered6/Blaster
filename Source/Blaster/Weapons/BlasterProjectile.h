// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterProjectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class BLASTER_API ABlasterProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABlasterProjectile();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> CollisionBoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|VFX")
	TObjectPtr<UParticleSystem> TracerParticleSystem;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TracerParticleSystemComponent;
};
