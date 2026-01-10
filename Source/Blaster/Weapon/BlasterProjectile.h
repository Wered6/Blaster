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

public:
	virtual void Destroyed() override;

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent,
	                   AActor* OtherActor,
	                   UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse,
	                   const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Damage")
	float Damage{20.f};

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> CollisionBoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|VFX")
	TObjectPtr<UParticleSystem> TracerParticleSystem;
	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TracerParticleSystemComponent;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|VFX")
	TObjectPtr<UParticleSystem> ImpactParticleSystem;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|SFX")
	TObjectPtr<USoundCue> ImpactSound;
};
