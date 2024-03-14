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

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
	// 따라서 서버에서 실행 되도록 코드를 짰는데, 그 영향을 받고 싶으면 reply에 넣어야 한다
	if (EquippedWeapon && Character)
	{
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
			HitTarget = End;
		}
		else
		{
			DrawDebugSphere(GetWorld() , OutResult.ImpactPoint, 12.f, 30 , FColor::Red);
			HitTarget = OutResult.ImpactPoint;
		}
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonpressed = bPressed;

	if (bFireButtonpressed)
	{
		ServerFire();
	}

}
void UCombatComponent::ServerFire_Implementation()
{
	MultiCastFire();
}

void UCombatComponent::MultiCastFire_Implementation()
{
	if (EquippedWeapon == nullptr) return;

	if (Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(HitTarget);
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult outResult;
	TraceUnderCrossHair(outResult);
}

void UCombatComponent::SetEquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* handWeaponSocket =  Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));


	if (handWeaponSocket)
	{
		handWeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	EquippedWeapon->SetOwner(Character);

	// 여기서 회전 방식을 바꿨을 떄는, SetEquipWeapon이 오로지 서버에서만 실행이 되기 떄문에 클라에는 적용이 안된다
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}



