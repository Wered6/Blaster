// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/HUD/BlasterCrosshairs.h"
#include "GameFramework/Actor.h"
#include "BlasterWeaponBase.generated.h"

class ABlasterCasing;
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
class BLASTER_API ABlasterWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ABlasterWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ShowPickUpWidget(const bool bShowWidget) const;

	void SetWeaponState(const EBlasterWeaponState State);

	virtual void Fire(const FVector& HitTargetLocation);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAreaSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                                      AActor* OtherActor,
	                                      UPrimitiveComponent* OtherComp,
	                                      int32 OtherBodyIndex,
	                                      bool bFromSweep,
	                                      const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnAreaSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	                                    AActor* OtherActor,
	                                    UPrimitiveComponent* OtherComp,
	                                    int32 OtherBodyIndex);

private:
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere, Category="Blaster|Weapon Properties")
	EBlasterWeaponState WeaponState;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Weapon Properties")
	TSubclassOf<ABlasterCasing> CasingClass;

#pragma region Zoom

public:
	FORCEINLINE float GetZoomedFOV() const
	{
		return ZoomedFOV;
	}

	FORCEINLINE float GetZoomInterpSpeed() const
	{
		return ZoomInterpSpeed;
	}

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Zoom")
	float ZoomedFOV{30.f};
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Zoom")
	float ZoomInterpSpeed{20.f};

#pragma endregion

#pragma region Crosshairs

public:
	FORCEINLINE FBlasterCrosshairsPackage GetCrosshairsPackage()
	{
		return CrosshairsPackage;
	}

private:
	/*
	 * Textures for the weapon crosshairs
	 */
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Crosshairs")
	FBlasterCrosshairsPackage CrosshairsPackage;

#pragma endregion

#pragma region Components

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMeshComponent() const
	{
		return WeaponMeshComponent;
	}

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> AreaSphereComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> PickUpWidgetComponent;

#pragma endregion
};
