// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/CombatComponent.h"
#include "character/BlasterCharacter.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

}

