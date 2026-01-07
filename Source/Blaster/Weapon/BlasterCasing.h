// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterCasing.generated.h"

UCLASS()
class BLASTER_API ABlasterCasing : public AActor
{
	GENERATED_BODY()

public:
	ABlasterCasing();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent,
	           AActor* OtherActor,
	           UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse,
	           const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Casing Properties")
	float ShellEjectionImpulse;
	
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Casing Properties")
	TObjectPtr<USoundCue> ShellSound;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CasingMeshComponent;
};
