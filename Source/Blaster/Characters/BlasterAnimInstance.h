// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

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

private:
	UPROPERTY(BlueprintGetter=GetSpeed, Category="Blaster|Movement")
	float Speed;
	UPROPERTY(BlueprintGetter=IsAirborne, Category="Blaster|Movement")
	bool bAirborne;
	UPROPERTY(BlueprintGetter=IsAccelerating, Category="Blaster|Movement")
	bool bAccelerating;
	UPROPERTY(BlueprintGetter=IsCrouched, Category="Blaster|Movement")
	bool bCrouched;

#pragma endregion

#pragma region Weapon

protected:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsAiming() const
	{
		return bAiming;
	}

private:
	UPROPERTY(BlueprintGetter=IsAiming, Category="Blaster|Weapon")
	bool bAiming;

#pragma endregion
};
