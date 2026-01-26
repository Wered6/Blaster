// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlasterCombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

enum class EBlasterWeaponType : uint8;
class IBlasterInteractWithCrosshairsInterface;
class ABlasterHUD;
class ABlasterPlayerController;
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

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TWeakObjectPtr<ABlasterCharacter> BlasterCharacter;
	TWeakObjectPtr<ABlasterPlayerController> BlasterPlayerController;
	TWeakObjectPtr<ABlasterHUD> BlasterHUD;

#pragma region Carried Ammo

public:
	FORCEINLINE int32 GetCarriedAmmo() const
	{
		return CarriedAmmo;
	}

private:
	void InitializeCarriedAmmo();

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Carried Ammo")
	int32 StartingARAmmo{30};

	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	TMap<EBlasterWeaponType, int32> CarriedAmmoMap;

#pragma endregion

#pragma region Weapon

public:
	void EquipWeapon(ABlasterWeaponBase* Weapon);
	void DropWeapon();

private:
	UFUNCTION()
	void OnRep_EquippedWeapon();

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	TObjectPtr<ABlasterWeaponBase> EquippedWeapon;

#pragma endregion

#pragma region Fire

private:
	void FireStart();
	void FireStop();
	void Fire();

	UFUNCTION(Server, Reliable)
	void Server_Fire(const FVector_NetQuantize& TraceHitTargetLocation);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Fire(const FVector_NetQuantize& TraceHitTargetLocation);

	void StartFireTimer();
	void OnFireTimerCompleted();

	bool CanFire() const;

	bool bFireButtonPressed;

	FTimerHandle FireTimer;

	bool bCanFire{true};

#pragma endregion

#pragma region Aim

private:
	void SetAiming(const bool bInAiming);
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(const bool bInAiming);

	UPROPERTY(Replicated)
	bool bAiming;

#pragma endregion

#pragma region Zoom

private:
	void InterpFOV(const float DeltaTime);

	float DefaultFOV;
	float CurrentFOV;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Zoom")
	float ZoomedFOV{30.f};
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Zoom")
	float ZoomInterpSpeed{20.f};

#pragma endregion

#pragma region Crosshairs

private:
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void SetCrosshairsSpread(const float DeltaTime);

	float CrosshairVelocityFactor;
	float CrosshairAirborneFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	float CrosshairAimAtPlayerFactor;

	IBlasterInteractWithCrosshairsInterface* TargetActor;

	FVector HitTargetLocation;

#pragma endregion

#pragma region Reload
	
public:
	void Reload();
	
private:
	UFUNCTION(Server, Reliable)
	void Server_Reload();

#pragma endregion

#pragma region Movement

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Movement")
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Movement")
	float AimWalkSpeed;

#pragma endregion
};
