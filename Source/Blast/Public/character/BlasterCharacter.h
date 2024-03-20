// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blast/BlasterTypes/TurningInPlace.h"
#include "Interaction/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLAST_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();

	void PlayHitMontage();
	void PlayElimMontage();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastElim();

	void Elim();


	virtual void OnRep_ReplicatedMovement() override;
protected:
	virtual void BeginPlay() override;

	void UpdateHUDHealth();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void Lookup(float Value);

	void Jump();
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();

	void AimOffset(float DeltaTime);
	void CalcAO_Pitch();
	void SimProxiesTurn();


	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	void PollInit();

	// Dissolve start

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;


	UFUNCTION()
	void UpdateDissolveTrack(float DissolveValue);

	void StartDissolve();


	UPROPERTY(EditAnyWhere, Category = "Elim")
	UCurveFloat* DissolveCurve;

	// Dynamic Material instance that we can change dusring runtime
	UPROPERTY(VisibleAnywhere, Category = "Elim")
		UMaterialInstanceDynamic* DynamicDissolveMaterial;

	//  Material instance that set in blueprint, used in dynamic instance
	UPROPERTY(EditAnyWhere, Category = "Elim")
	UMaterialInstance* DissolveMaterial;

	// Dissolve end

private:
	UPROPERTY(VisibleAnyWhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnyWhere, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;


	// 이 함수는 replicate 된 경우, 자동 호출 되는 함수이기에, 어떻게 보면 서버 자체는 실행하지 못하는 함수이다
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnyWhere)
	class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();



	float AO_Yaw;
	// 임계점에 도달해서 돌아야 할 떄는 -90까지 갔던 것이 0으로 (정면으로) 돌아가야 한다. 그떄 보간에 사용하기 위한 변수. 
	float Interp_AO_Yaw;
	float AO_Pitch;

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);


	UPROPERTY(EditAnyWhere, Category = "Combat")
		class UAnimMontage* FireWeaponMontage;

		UPROPERTY(EditAnyWhere, Category = "Combat")
	class UAnimMontage* ReloadMontage;


	UPROPERTY(EditAnyWhere, Category = "Combat")
		class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnyWhere, Category = "Combat")
		class UAnimMontage* ElimMontage;



	UPROPERTY(EditAnyWhere, Category = "Combat")
		float CameraThreshold = 200.f;

	float TurnThreshold = 0.5f;
	FRotator ProxyRotatinLastFrame;
	FRotator ProxyRotatin;
	float ProxyYaw;

	float TimeSinceLastMovementReplication;

	bool bRotateRootBone;

	void HideCameraIfCharacterIsClose();

	float CalcualteSpeed();


	UPROPERTY(EditAnyWhere, Category = "Player Stats")
		float MaxHealth = 100.f;

	UPROPERTY(EditAnyWhere, ReplicatedUsing = OnRep_Health, Category = "Player Stats")
		float Health = 100.f;

	bool bElimed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditAnyWhere)
		float ElimDelay = 3.f;

	void ElimTimerFinished();


	UFUNCTION()
		void OnRep_Health();

	UPROPERTY()
	class ABlastPlayerController* BlastPlayerController;

	UPROPERTY()
	class ABlasterPlayerState* BlasterplayerState;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool isWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() { return TurningInPlace; }
	FVector GetHitTarget();
	FORCEINLINE UCameraComponent* GetFollowCamera() { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimed() const { return bElimed; }

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }


};
