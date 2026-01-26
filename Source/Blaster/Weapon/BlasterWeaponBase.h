// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCasing.h"
#include "Blaster/HUD/BlasterCrosshairs.h"
#include "GameFramework/Actor.h"
#include "BlasterWeaponBase.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;
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

protected:
	virtual void BeginPlay() override;

public:
	void ShowPickUpWidget(const bool bShowWidget) const;

	void SetWeaponState(const EBlasterWeaponState State);

	void Drop();

	virtual void SetOwner(AActor* NewOwner) override;

private:
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_WeaponState, Category="Blaster|Weapon Properties")
	EBlasterWeaponState WeaponState;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Weapon Properties")
	TSubclassOf<ABlasterCasing> CasingClass;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<ABlasterCharacter> BlasterOwnerCharacter;
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<ABlasterPlayerController> BlasterOwnerPlayerController;

#pragma region Ammo

public:
	FORCEINLINE bool IsEmpty() const
	{
		return Ammo <= 0;
	}

private:
	void SpendRound();

	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_Ammo, Category="Blaster|Ammo")
	int32 Ammo;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Ammo")
	int32 MagCapacity;

#pragma endregion

#pragma region Fire

public:
	virtual void Fire(const FVector& HitTargetLocation);

	FORCEINLINE float GetFireDelay() const
	{
		return FireDelay;
	}

	FORCEINLINE bool IsAutomatic() const
	{
		return bAutomatic;
	}

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Fire")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Fire")
	float FireDelay{.15f};
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Fire")
	bool bAutomatic{true};

#pragma endregion

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

protected:
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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> AreaSphereComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> PickUpWidgetComponent;

#pragma endregion
};
