// Fill out your copyright notice in the Description page of Project Settings.


#include "character/BlasterAnimInstance.h"
#include "character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	bIsCrouched = BlasterCharacter->bIsCrouched;
	bAiming = BlasterCharacter->IsAiming();

	// offset yaw for strafing

	// 이게 컨트롤러 가 보고있는, 즉 카메라가 보고 있는 정면의 rotation
	FRotator AimRotation =  BlasterCharacter->GetBaseAimRotation();
	// 지금 이동하는 방향을 월드 기준으로 rotation으로 바꿈
	FRotator MovementeRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());

	// 이건 어떻게 보면 aim rotation을 기준으로 보고, 그 기준으로 봤을 떄 MovementeRotation가 어느정도 각도인지를 알아내는 것이다
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementeRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation , DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	//YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementeRotation, AimRotation).Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();

	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);

	Lean = FMath::Clamp(Interp , -90.f, 90.f);
}
