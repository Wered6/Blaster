// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter(ViewportSize / 2.f);

	if (CrosshairsPackage.AreValid())
	{
		DrawCrosshair(CrosshairsPackage.CrosshairCenter, ViewportCenter);
		DrawCrosshair(CrosshairsPackage.CrosshairLeft, ViewportCenter);
		DrawCrosshair(CrosshairsPackage.CrosshairRight, ViewportCenter);
		DrawCrosshair(CrosshairsPackage.CrosshairTop, ViewportCenter);
		DrawCrosshair(CrosshairsPackage.CrosshairBottom, ViewportCenter);
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	const float TextureWidth{Texture->GetSurfaceWidth()};
	const float TextureHeight{Texture->GetSurfaceHeight()};
	const FVector2D TextureDrawPoint{ViewportCenter.X - TextureWidth / 2.f, ViewportCenter.Y - TextureHeight / 2.f};

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
		FLinearColor::White
	);
}
