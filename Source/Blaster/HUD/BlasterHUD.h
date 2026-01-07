// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCrosshairs.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	FORCEINLINE void SetCrosshairsPackage(const FBlasterCrosshairsPackage& Package)
	{
		CrosshairsPackage = Package;
	}

	FORCEINLINE void SetCrosshairSpread(const float Spread)
	{
		CrosshairsPackage.CrosshairSpread = Spread;
	}

private:
	void DrawCrosshair(UTexture2D* Texture, const FVector2D ViewportCenter, const FVector2D Spread);

	FBlasterCrosshairsPackage CrosshairsPackage;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Crosshair")
	float CrosshairSpreadMax{16.f};
};
