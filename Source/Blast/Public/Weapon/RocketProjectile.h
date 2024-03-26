// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Projectile.h"
#include "RocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class BLAST_API ARocketProjectile : public AProjectile
{
	GENERATED_BODY()
	
public:
	ARocketProjectile();

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* otherActor, UPrimitiveComponent* ohterComp, FVector normalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;


	UPROPERTY(EditAnywhere)
	class USoundCue* ProjectileLoop;

	UPROPERTY(EditAnywhere)
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	class USoundAttenuation* LoopSoundAttenuation;


};
