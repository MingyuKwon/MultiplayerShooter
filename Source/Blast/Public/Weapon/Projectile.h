// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLAST_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* otherActor, UPrimitiveComponent* ohterComp, FVector normalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnyWhere)
	float Damage = 20.f;

	UPROPERTY(VisibleAnyWhere)
		class UProjectileMovementComponent* ProjectileMovementComponent;


private:
	UPROPERTY(EditAnyWhere)
	class UBoxComponent* CollisionBox;


	UPROPERTY(EditAnyWhere)
	UParticleSystem* Trace;

	UPROPERTY(EditAnyWhere)
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnyWhere)
	class USoundCue* ImpactSound;

	class UParticleSystemComponent* TraceComponent;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
