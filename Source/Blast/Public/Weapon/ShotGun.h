// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class BLAST_API AShotGun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& hitTarget) override;

	UPROPERTY(EditAnywhere)
	int32 NumberofBalls = 10;
};
