// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCrosshairs.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UBlasterAnnouncementWidget;
class UBlasterCharacterOverlay;

DECLARE_MULTICAST_DELEGATE(FOnCharacterOverlayInitializedSignature)

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

#pragma region Announcement

public:
	void AddAnnouncementWidget();

	FORCEINLINE UBlasterAnnouncementWidget* GetAnnouncementWidget() const
	{
		return AnnouncementWidget;
	}
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Announcement")
	TSubclassOf<UBlasterAnnouncementWidget> AnnouncementWidgetClass;

	UPROPERTY()
	TObjectPtr<UBlasterAnnouncementWidget> AnnouncementWidget;

#pragma endregion

#pragma region Character Overlay

public:
	void AddCharacterOverlay();
	void RemoveCharacterOverlay() const;

	FORCEINLINE UBlasterCharacterOverlay* GetCharacterOverlay() const
	{
		return CharacterOverlay;
	}

	FOnCharacterOverlayInitializedSignature OnCharacterOverlayInitializedDelegate;

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Character Overlay")
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

	FORCEINLINE void ResetCrosshairsPackage()
	{
		CrosshairsPackage.Reset();
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
