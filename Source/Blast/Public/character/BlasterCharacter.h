// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLAST_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void Lookup(float Value);

	void Jump();
	void EquipButtonPressed();

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

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool isWeaponEquipped();
};
