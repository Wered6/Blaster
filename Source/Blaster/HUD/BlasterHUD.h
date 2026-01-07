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

private:
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);
	
	FBlasterCrosshairsPackage CrosshairsPackage;
};
