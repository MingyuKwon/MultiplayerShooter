// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Projectile.h"

void AProjectileWeapon::Fire(const FVector& hitTarget)
{
	Super::Fire(hitTarget);

	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* muzzleSocket =  GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (muzzleSocket)
	{
		FTransform socketTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTargetDirection = hitTarget - socketTransform.GetLocation();

		FActorSpawnParameters spawnParameters;
		spawnParameters.Owner = GetOwner();
		spawnParameters.Instigator = InstigatorPawn;

		UWorld* world = GetWorld();

		if (world)
		{
			world->SpawnActor<AProjectile>(
				projectileClass,
				socketTransform.GetLocation(),
				ToTargetDirection.Rotation(),
				spawnParameters
			);
		}
	}
	
}
