// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCrosshairs.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UBlasterCharacterOverlay;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

#pragma region CharacterOverlay

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Overlay")
	TSubclassOf<UBlasterCharacterOverlay> CharacterOverlayClass;

	UPROPERTY()
	TObjectPtr<UBlasterCharacterOverlay> CharacterOverlay;

#pragma endregion

#pragma region Crosshairs

public:
	virtual void DrawHUD() override;

	FORCEINLINE void SetCrosshairsPackage(const FBlasterCrosshairsPackage& Package)
	{
		CrosshairsPackage = Package;
	}

	FORCEINLINE void SetCrosshairsSpread(const float Spread)
	{
		CrosshairsPackage.CrosshairSpread = Spread;
	}

	FORCEINLINE void SetCrosshairsColor(const FLinearColor CrosshairsColor)
	{
		CrosshairsPackage.CrosshairColor = CrosshairsColor;
	}

private:
	void DrawCrosshair(UTexture2D* Texture, const FVector2D ViewportCenter, const FVector2D Spread, FLinearColor CrosshairsColor);

	FBlasterCrosshairsPackage CrosshairsPackage;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Crosshairs")
	float CrosshairSpreadMax{16.f};

#pragma endregion
};
