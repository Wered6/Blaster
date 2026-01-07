// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCrosshairs.generated.h"

USTRUCT(BlueprintType)
struct FBlasterCrosshairsPackage
{
	GENERATED_BODY()

	bool AreValid() const
	{
		return CrosshairCenter && CrosshairLeft && CrosshairRight && CrosshairTop && CrosshairBottom;
	}

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairCenter;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairLeft;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairRight;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairTop;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairBottom;
	
	float CrosshairSpread;
};
