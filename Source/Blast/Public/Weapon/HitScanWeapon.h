// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLAST_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& hitTarget) override;

protected:
	UPROPERTY(EditAnywhere)
		float Damage = 10.f;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* BeamParticle;

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

private:
	
	// Trace end with scater
	UPROPERTY(EditAnywhere)
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere)
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
	bool bUseScatter = false;

};
