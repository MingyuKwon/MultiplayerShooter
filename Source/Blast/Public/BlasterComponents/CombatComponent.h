// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000


class ABlasterCharacter;
class AWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLAST_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 외부에서 모든 접근을 허용할 클래스
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetEquipWeapon(AWeapon* WeaponToEquip);

	void FireButtonPressed(bool bPressed);

	FORCEINLINE FVector GetHitTarget() { return HitTarget; }


protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// 이건 어디에서 호출이 되도, 실제 실행을 서버 월드에서 실행함
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	// 이건 클라이언트에서 호출 되면 그냥 자기 자신에서 실행이지만, 서버에서 호출시 서버와 모든 연결된 클라이언트에서 실행
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrossHair(FHitResult& OutResult);


	void SetHUDCrosshairs(float DeltaTime);

private:

	ABlasterCharacter* Character;
	class ABlastPlayerController* playerController;
	class ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnyWhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnyWhere)
	float AimWalkSpeed;

	bool bFireButtonpressed;

	// HUD and Crosshair
	float CrosshairVelocityVector;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;


	FVector HitTarget;

	// Aiming and FOV
	UPROPERTY(EditAnyWhere)
	float DefaultFOV;

	UPROPERTY(EditAnyWhere,  Category = "Combat")
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnyWhere, Category = "Combat")
	float ZoomedInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);
};
