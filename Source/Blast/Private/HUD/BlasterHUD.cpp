// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewPortCenter(ViewportSize.X/2.f, ViewportSize.Y/2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshiar(HUDPackage.CrosshairCenter, ViewPortCenter, Spread);
		}

		if (HUDPackage.CrosshairLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshiar(HUDPackage.CrosshairLeft, ViewPortCenter, Spread);
		}

		if (HUDPackage.CrosshairRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshiar(HUDPackage.CrosshairRight, ViewPortCenter, Spread);
		}

		if (HUDPackage.CrosshairTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshiar(HUDPackage.CrosshairTop, ViewPortCenter, Spread);
		}

		if (HUDPackage.CrosshairBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshiar(HUDPackage.CrosshairBottom, ViewPortCenter, Spread);
		}
	}
}

void ABlasterHUD::DrawCrosshiar(UTexture2D* Texture, FVector2D ViewportCententer, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportCententer.X - TextureWidth / 2.f + Spread.X,
		ViewportCententer.Y - TextureHeight / 2.f + Spread.Y
	);

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
