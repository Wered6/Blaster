// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlasterCombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

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

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Fire();
	UFUNCTION(Server, Reliable)
	void Server_Fire();
	
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> BlasterCharacter;
	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	TObjectPtr<ABlasterWeaponBase> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere, Category="Blaster|Movement")
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere, Category="Blaster|Movement")
	float AimWalkSpeed;

	bool bFireButtonPressed;
};
