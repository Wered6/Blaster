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

#pragma region Character

protected:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	ABlasterCharacter* GetBlasterCharacter()
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
	float GetSpeed() const
	{
		return Speed;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	bool IsAirborne() const
	{
		return bAirborne;
	}

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, meta=(BlueprintThreadSafe))
	bool IsAccelerating() const
	{
		return bAccelerating;
	}

private:
	UPROPERTY(BlueprintGetter=GetSpeed, Category="Blaster|Movement")
	float Speed;
	UPROPERTY(BlueprintGetter=IsAirborne, Category="Blaster|Movement")
	bool bAirborne;
	UPROPERTY(BlueprintGetter=IsAccelerating, Category="Blaster|Movement")
	bool bAccelerating;

#pragma endregion
};
