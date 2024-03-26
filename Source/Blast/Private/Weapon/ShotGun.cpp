// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ShotGun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AShotGun::Fire(const FVector& hitTarget)
{
	AWeapon::Fire(hitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

		FVector Start = SocketTransform.GetLocation();

		TMap<ABlasterCharacter*, int32> Hitmap;
		for (int32 i = 0; i < NumberofBalls; i++)
		{
			FHitResult fireResult;
			WeaponTraceHit(Start, hitTarget, fireResult);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(fireResult.GetActor());
			if (BlasterCharacter && HasAuthority() && InstigatorController)
			{
				if (Hitmap.Contains(BlasterCharacter))
				{
					Hitmap[BlasterCharacter]++;
				}
				else
				{
					Hitmap.Emplace(BlasterCharacter, 1);
				}
			}

			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticle,
					fireResult.ImpactPoint,
					fireResult.ImpactNormal.Rotation()
				);
			}
		}


		for (auto HitPair : Hitmap)
		{
			if (InstigatorController && HitPair.Key && HasAuthority())
			{
				UGameplayStatics::ApplyDamage(
					HitPair.Key,
					Damage * HitPair.Value,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			
		}

		

	}
}
