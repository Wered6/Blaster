// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

enum class ETurningInPlace : uint8;
class ABlasterWeaponBase;
class ABlasterCharacter;

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

#pragma region Weapon

protected:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	bool IsWeaponEquipped() const
	{
		return bWeaponEquipped;
	}

private:
	UPROPERTY(BlueprintGetter=IsWeaponEquipped, Category="Blaster|Weapon")
	bool bWeaponEquipped{};

	TWeakObjectPtr<ABlasterWeaponBase> EquippedWeapon;

#pragma endregion

#pragma region Character

protected:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	ABlasterCharacter* GetBlasterCharacter() const
	{
		return BlasterCharacter;
	}

private:
	UPROPERTY(BlueprintGetter=GetBlasterCharacter, Category="Blaster|Character")
	TObjectPtr<ABlasterCharacter> BlasterCharacter;

#pragma endregion

#pragma region Movement

protected:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const
	{
		return Speed;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsAirborne() const
	{
		return bAirborne;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsAccelerating() const
	{
		return bAccelerating;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsCrouched() const
	{
		return bCrouched;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsAiming() const
	{
		return bAiming;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawOffset() const
	{
		return YawOffset;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLean() const
	{
		return Lean;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawAimOffset() const
	{
		return YawAimOffset;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetPitchAimOffset() const
	{
		return PitchAimOffset;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE FTransform GetLeftHandTransform() const
	{
		return LeftHandTransform;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE ETurningInPlace GetTurningInPlace() const
	{
		return TurningInPlace;
	}

private:
	UPROPERTY(BlueprintGetter=GetSpeed, Category="Blaster|Movement")
	float Speed;
	UPROPERTY(BlueprintGetter=IsAirborne, Category="Blaster|Movement")
	bool bAirborne;
	UPROPERTY(BlueprintGetter=IsAccelerating, Category="Blaster|Movement")
	bool bAccelerating;
	UPROPERTY(BlueprintGetter=IsCrouched, Category="Blaster|Movement")
	bool bCrouched;
	UPROPERTY(BlueprintGetter=IsAiming, Category="Blaster|Weapon")
	bool bAiming;

	UPROPERTY(BlueprintGetter=GetYawOffset, Category="Blaster|Movement")
	float YawOffset;
	UPROPERTY(BlueprintGetter=GetLean, Category="Blaster|Movement")
	float Lean;

	FRotator LastFrameRotation;
	FRotator CurrentRotation;
	FRotator DeltaRotation;

	UPROPERTY(BlueprintGetter=GetYawAimOffset, Category="Blaster|Movement")
	float YawAimOffset;
	UPROPERTY(BlueprintGetter=GetPitchAimOffset, Category="Blaster|Movement")
	float PitchAimOffset;

	UPROPERTY(BlueprintGetter=GetLeftHandTransform, Category="Blaster|Movement")
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintGetter=GetTurningInPlace, Category="Blaster|Movement")
	ETurningInPlace TurningInPlace;

#pragma endregion
};
