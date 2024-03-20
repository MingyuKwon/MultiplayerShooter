// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/CombatComponent.h"
#include "character/BlasterCharacter.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Controller/BlastPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Interaction/InteractWithCrosshairsInterface.h"
#include "TimerManager.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 350.f;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, CarriedAmmo);

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}

		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult result;
		TraceUnderCrossHair(result);
		HitTarget = result.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}

}


void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomedInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomedInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}

}


void UCombatComponent::SetAiming(bool bIsAiming)
{
	// 본래는 이걸 서버 인지 아닌지 확인해서 호출하는 것이 기본이지만, 이 조준은 예측 적인 요소를 추가해서 서버의 허락을 받지 않아도 미리 바꾸게 한다
	bAiming = bIsAiming;

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}

	ServerSetAiming(bIsAiming);

}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	FString str = FString::Printf(TEXT("OnRep_EquippedWeapon called"));
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, *str);


	// 따라서 서버에서 실행 되도록 코드를 짰는데, 그 영향을 받고 싶으면 reply에 넣어야 한다
	if (EquippedWeapon && Character)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, *str);


		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* handWeaponSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (handWeaponSocket)
		{
			handWeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::TraceUnderCrossHair(FHitResult& OutResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D ViewPortCentor(ViewportSize.X / 2, ViewportSize.Y / 2);
	
	FVector CrossHairLocation;
	FVector CrossHairDirection;

	bool bScreentoWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		ViewPortCentor,
		CrossHairLocation,
		CrossHairDirection
	);


	if (bScreentoWorld)
	{
		FVector Start = CrossHairLocation;

		if (Character)
		{
			float DistancetoCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrossHairDirection * (DistancetoCharacter + 100.f);
		}

		FVector End = Start + CrossHairDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			OutResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!OutResult.bBlockingHit)
		{
			OutResult.ImpactPoint = End;
		}


		if (OutResult.GetActor() && OutResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}

	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);

}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr ) return;

	bCanFire = true;
	if (bFireButtonpressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;

	return !EquippedWeapon->IsAmmoEmpty() && bCanFire;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	playerController = playerController == nullptr ? Cast<ABlastPlayerController>(Character->Controller) : playerController;
	if (playerController)
	{
		playerController->SetCarriedAmmoHUD(CarriedAmmo);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
}


void UCombatComponent::FireButtonPressed(bool bPressed)
{
	if (EquippedWeapon == nullptr) return;

	bFireButtonpressed = bPressed;

	if (bFireButtonpressed)
	{
		Fire();
	}

}
void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		ServerFire(HitTarget);

		if (EquippedWeapon)
		{
			CrosshairShootFactor = 0.75f;
		}

		StartFireTimer();
	}
	
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0)
	{
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr) return;

	Character->PlayReloadMontage();
}


void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}

void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;

	if (Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}


void UCombatComponent::SetEquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* handWeaponSocket =  Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (handWeaponSocket)
	{
		handWeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetAmmoHUD();
	
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	playerController = playerController == nullptr ? Cast<ABlastPlayerController>(Character->Controller) : playerController;
	if (playerController)
	{
		playerController->SetCarriedAmmoHUD(CarriedAmmo);
	}

	// 여기서 회전 방식을 바꿨을 떄는, SetEquipWeapon이 오로지 서버에서만 실행이 되기 떄문에 클라에는 적용이 안된다
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	playerController = playerController == nullptr ? Cast<ABlastPlayerController>(Character->Controller) : playerController;

	if (playerController)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(playerController->GetHUD()) : HUD;
		if (HUD)
		{
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairCenter;

			}
			else
			{
				HUDPackage.CrosshairBottom = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairCenter = nullptr;
			}

			// calculate crosshair spread
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityVector = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 1.5f, DeltaTime, 2.f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread = 0.5f + CrosshairVelocityVector + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}



