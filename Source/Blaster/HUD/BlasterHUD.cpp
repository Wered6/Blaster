// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"
#include "BlasterCharacterOverlay.h"

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(CharacterOverlayClass))
	{
		return;
	}

	CharacterOverlay = CreateWidget<UBlasterCharacterOverlay>(GetOwningPlayerController(), CharacterOverlayClass);
	CharacterOverlay->AddToViewport();
}

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter(ViewportSize / 2.f);

	const float SpreadScaled{CrosshairSpreadMax * CrosshairsPackage.CrosshairSpread};

	if (CrosshairsPackage.AreValid())
	{
		DrawCrosshair(CrosshairsPackage.CrosshairCenter, ViewportCenter, FVector2D(0.f, 0.f), CrosshairsPackage.CrosshairColor);
		DrawCrosshair(CrosshairsPackage.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f), CrosshairsPackage.CrosshairColor);
		DrawCrosshair(CrosshairsPackage.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0.f), CrosshairsPackage.CrosshairColor);
		DrawCrosshair(CrosshairsPackage.CrosshairTop, ViewportCenter, FVector2D(0.f, -SpreadScaled), CrosshairsPackage.CrosshairColor);
		DrawCrosshair(CrosshairsPackage.CrosshairBottom, ViewportCenter, FVector2D(0.f, SpreadScaled), CrosshairsPackage.CrosshairColor);
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D ViewportCenter, const FVector2D Spread, const FLinearColor CrosshairsColor)
{
	const float TextureWidth{Texture->GetSurfaceWidth()};
	const float TextureHeight{Texture->GetSurfaceHeight()};
	const FVector2D TextureDrawPoint{
		ViewportCenter.X - TextureWidth / 2.f + Spread.X,
		ViewportCenter.Y - TextureHeight / 2.f + Spread.Y
	};

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairsColor
	);
}
