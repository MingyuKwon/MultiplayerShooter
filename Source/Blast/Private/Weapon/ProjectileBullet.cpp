// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"


AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("UProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* otherActor, UPrimitiveComponent* ohterComp, FVector normalImpulse, const FHitResult& Hit)
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());

	if (CharacterOwner)
	{
		AController* controller = CharacterOwner->Controller;
		if (controller)
		{
			UGameplayStatics::ApplyDamage(otherActor, Damage, controller, this, UDamageType::StaticClass());
		}
	}


	// Call Destroy
	Super::OnHit(HitComp , otherActor, ohterComp, normalImpulse, Hit);
}
