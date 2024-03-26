// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RocketProjectile.h"
#include "Kismet/GameplayStatics.h"

ARocketProjectile::ARocketProjectile()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Skeletal Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* otherActor, UPrimitiveComponent* ohterComp, FVector normalImpulse, const FHitResult& Hit)
{
	APawn* instigator = GetInstigator();

	if (instigator)
	{
		AController* controller = instigator->GetController();
		if (controller)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				Damage,
				10.f,
				GetActorLocation(),
				200.f,
				500.f,
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				controller
			);
		}
	}

	Super::OnHit(HitComp, otherActor, ohterComp, normalImpulse, Hit);
}
