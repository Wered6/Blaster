// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlasterCombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

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

	void EquipWeapon(ABlasterWeaponBase* Weapon);

protected:
	void SetAiming(const bool bInAiming);

	UFUNCTION(Server, Reliable)
	void Server_SetAiming(const bool bInAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireStart();
	void FireStop();

	UFUNCTION(Server, Reliable)
	void Server_Fire(const FVector_NetQuantize& TraceHitTargetLocation);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Fire(const FVector_NetQuantize& TraceHitTargetLocation);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult) const;

private:
	TWeakObjectPtr<ABlasterCharacter> BlasterCharacter;
	TWeakObjectPtr<ABlasterPlayerController> BlasterPlayerController;
	TWeakObjectPtr<ABlasterHUD> BlasterHUD;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	TObjectPtr<ABlasterWeaponBase> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Movement")
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Movement")
	float AimWalkSpeed;

	bool bFireButtonPressed;

	FVector HitTargetLocation;

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
	void SetCrosshairsSpread(const float DeltaTime);

	float CrosshairVelocityFactor;
	float CrosshairAirborneFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

#pragma endregion
};
