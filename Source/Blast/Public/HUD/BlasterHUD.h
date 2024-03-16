// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
	class UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
	UTexture2D* CrosshairTop;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
	UTexture2D* CrosshairBottom;
};

/**
 * 
 */
UCLASS()
class BLAST_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;

	void DrawCrosshiar(UTexture2D* Texture, FVector2D ViewportCententer);
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& package) {HUDPackage = package;}
};
