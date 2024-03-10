// Fill out your copyright notice in the Description page of Project Settings.


#include "character/BlasterAnimInstance.h"
#include "character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	speed = Velocity.Size();

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsInAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
	bWeaponEquipped = BlasterCharacter->isWeaponEquipped();
}
