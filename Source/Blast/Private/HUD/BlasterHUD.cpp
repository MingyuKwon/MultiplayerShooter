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

		if (HUDPackage.CrosshairCenter)
		{
			DrawCrosshiar(HUDPackage.CrosshairCenter, ViewPortCenter);
		}

		if (HUDPackage.CrosshairLeft)
		{
			DrawCrosshiar(HUDPackage.CrosshairLeft, ViewPortCenter);
		}

		if (HUDPackage.CrosshairRight)
		{
			DrawCrosshiar(HUDPackage.CrosshairRight, ViewPortCenter);
		}

		if (HUDPackage.CrosshairTop)
		{
			DrawCrosshiar(HUDPackage.CrosshairTop, ViewPortCenter);
		}

		if (HUDPackage.CrosshairBottom)
		{
			DrawCrosshiar(HUDPackage.CrosshairBottom, ViewPortCenter);
		}
	}
}

void ABlasterHUD::DrawCrosshiar(UTexture2D* Texture, FVector2D ViewportCententer)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportCententer.X - TextureWidth / 2.f,
		ViewportCententer.Y - TextureHeight / 2.f
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
