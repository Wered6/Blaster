// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterWeapon.generated.h"

class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EBlasterWeaponState : uint8
{
	Initial UMETA(DisplayName="Initial State"),
	Equipped UMETA(DisplayName="Equipped"),
	Dropped UMETA(DisplayName="Dropped"),

	MAX UMETA(DisplayName="DefaultMAX")
};

UCLASS()
class BLASTER_API ABlasterWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABlasterWeapon();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent,
	                                 AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp,
	                                 int32 OtherBodyIndex,
	                                 bool bFromSweep,
	                                 const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, Category="Blaster|Weapon Properties")
	EBlasterWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphereComponent;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Weapon Properties")
	TObjectPtr<UWidgetComponent> PickUpWidgetComponent;
};
