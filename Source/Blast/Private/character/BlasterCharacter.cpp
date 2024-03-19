// Fill out your copyright notice in the Description page of Project Settings.


#include "character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "character/BlasterAnimInstance.h"
#include "BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blast/Blast.h"
#include "Controller/BlastPlayerController.h"
#include "GameMode/BlasterGameMode.h"
#include "TimerManager.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHead Widget"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	Combat = CreateDefaultSubobject<UCombatComponent>(FName("Combat Component"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 600.f);

	TurningInPlace = ETurningInPlace::ERIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;


	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dissolve TImeline"));
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME 만 하면, 서버에서 그 누구가 weapon을 바꾸기만 하면 모든 blastChaacter들이 영향을 받는다. 영향을 받는 범위를 제한하고 싶다면 CONDITION이 추가된 매크로를 사용한다
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);

}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}

}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr) return;
	if (Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && FireWeaponMontage)
	{
		animInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleHip") : FName("RifleAim");
		animInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && ElimMontage)
	{
		animInstance->Montage_Play(ElimMontage);
	}
}



void ABlasterCharacter::PlayHitMontage()
{
	if (Combat == nullptr) return;
	if (Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && HitReactMontage)
	{
		animInstance->Montage_Play(HitReactMontage);
		FName SectionName = FName("FromRront");
		animInstance->Montage_JumpToSection(SectionName);
	}
}


void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlastPlayerController = BlastPlayerController == nullptr ? Cast<ABlastPlayerController>(Controller) : BlastPlayerController;

	if (BlastPlayerController)
	{
		BlastPlayerController->SetHealthHUD(Health, MaxHealth);
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0, MaxHealth);

	// 어짜피 모든 데미지 연산은 서버에서 일어나기에 이 Reveive damage는 서버에서만 일어난다
	UpdateHUDHealth();
	PlayHitMontage();

	if (Health <= 0.f)
	{
		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode)
		{
			BlastPlayerController = BlastPlayerController == nullptr ? Cast<ABlastPlayerController>(Controller) : BlastPlayerController;
			ABlastPlayerController* KillingPlayerController = Cast<ABlastPlayerController>(InstigatedBy);

			BlasterGameMode->PlayerEliminated(this, KillingPlayerController, BlastPlayerController);
		}
	}

}


void ABlasterCharacter::UpdateDissolveTrack(float DissolveValue)
{
	if (DynamicDissolveMaterial)
	{
		DynamicDissolveMaterial->SetScalarParameterValue(TEXT("DissolveRate"), DissolveValue);

	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveTrack);
	if (DissolveCurve)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::OnRep_Health()
{
	// 이건 replicate 된 health에 반응해서 일어나느 ㄴ것이므로 클라이언트 에서만 실행 된다
	UpdateHUDHealth();
	PlayHitMontage();
}


void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 여기서 자기가 authority, autonomous 라면 에임 오프셋을 쫓고, 그에 맞춰 회전을 시키고, 돌지만
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalcAO_Pitch();
	}

	HideCameraIfCharacterIsClose();

}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), EInputEvent::IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Released, this, &ABlasterCharacter::FireButtonReleased);


	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("Lookup"), this, &ABlasterCharacter::Lookup);

}

void ABlasterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::Lookup(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		ACharacter::Jump();
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->SetEquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}

}

void ABlasterCharacter::AimButtonPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

float ABlasterCharacter::CalcualteSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();

}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	float speed = CalcualteSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// 에임 오프셋은, 우리가 움직일 떄는 적용하지 않고 가만히 서있으면서 다른 방향을 조준 할 때 사용한다.
	// 따라서 딱 멈췄을 떄 정면이 어디인지 계속 저장해 두고 ,멈춘 경우 현재 보고 있는 방향이 멈췄을 떄 방향과 얼마 정도 차이가 나는지를 계산해서 aimoffset 에게 인수로 줘야 한다

	if (speed == 0.f && !bIsInAir) // Standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ERIP_NotTurning)
		{
			Interp_AO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	else // running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ERIP_NotTurning;
	}

	// Pith는 이동과 별 상관이 없으므로 그냥 해도 됨
	// 이 값이 서버로 갈 때 서버는 회전 각도를 0~360으로 처리하는 문제가 있다
	// 우리는 ao-pitch의 값이 -90~90이라고 생각하고 애니메이션을 짰는데 서버로 전송되어서 압축 해제 된 값은 0~360이었던 것
	CalcAO_Pitch();

	//AO_Yaw 는 애초에 원래 있던 위치와 비교해서 각도를 정하니까 이런 문제가 없는데 pitch는 그냥 회전 값을 생으로 가져와서 사용해서 이런 문제가 생긴다
}

void ABlasterCharacter::CalcAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// Map pitch from 9270 , 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		// 그래서 서버 측이면 0~360 범위라면 그 값을 다시 정정 시켜줘야 한다
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr) return;
	if (Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;

	float speed = CalcualteSpeed();
	if (speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ERIP_NotTurning;
		return;
	}

	ProxyRotatinLastFrame = ProxyRotatin;
	ProxyRotatin = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotatin, ProxyRotatinLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ERIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ERIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ERIP_NotTurning;

		}

		return;
	}

	TurningInPlace = ETurningInPlace::ERIP_NotTurning;


}

void ABlasterCharacter::Elim()
{
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ABlasterCharacter::ElimTimerFinished,
		ElimDelay
	);
}


void ABlasterCharacter::MulticastElim_Implementation()
{
	bElimed = true;
	PlayElimMontage();

	if (DissolveMaterial)
	{
		DynamicDissolveMaterial = UMaterialInstanceDynamic::Create(DissolveMaterial, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterial);

		DynamicDissolveMaterial->SetScalarParameterValue(TEXT("DissolveRate"), -0.55f);
		DynamicDissolveMaterial->SetScalarParameterValue(TEXT("Glow"), 100.f);

	}

	StartDissolve();
}

void ABlasterCharacter::ElimTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (BlasterGameMode)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}

}


void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->SetEquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ERIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ERIP_Left;

	}

	if (TurningInPlace != ETurningInPlace::ERIP_NotTurning)
	{
		Interp_AO_Yaw = FMath::FInterpTo(Interp_AO_Yaw, 0.f, DeltaTime, 10.f);
		AO_Yaw = Interp_AO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ERIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}


void ABlasterCharacter::HideCameraIfCharacterIsClose()
{
	if (!IsLocallyControlled()) return;

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon)
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon)
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}


void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}

}

bool ABlasterCharacter::isWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget()
{
	if (Combat == nullptr) return FVector();

	return Combat->HitTarget;
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	// 이게 null이면 가리기가 안되는데
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	// 이렇게 콜백에 last 넣어주면 null로 변형 된 후에도 접근이 가능
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}

}



