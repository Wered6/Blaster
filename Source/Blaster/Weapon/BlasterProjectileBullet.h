// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterProjectile.h"
#include "BlasterProjectileBullet.generated.h"

UCLASS()
class BLASTER_API ABlasterProjectileBullet : public ABlasterProjectile
{
	GENERATED_BODY()

public:
	ABlasterProjectileBullet();

protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent,
	                   AActor* OtherActor,
	                   UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse,
	                   const FHitResult& Hit) override;
};
