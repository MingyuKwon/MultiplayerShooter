// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial,
	EWS_Equipped,
	EWS_Dropped,


	EWS_Max
};



UCLASS()
class BLAST_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWIdget);

	virtual void Fire(const FVector& hitTarget);

	virtual void Dropped();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetAmmoHUD();


	// Testures for the weapon crosshair

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
		class UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
		UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
		UTexture2D* CrosshairRight;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
		UTexture2D* CrosshairTop;

	UPROPERTY(EditAnyWhere, Category = "CrossHairs")
		UTexture2D* CrosshairBottom;


	// Automatic FIre
	UPROPERTY(EditAnyWhere, Category = "Combat")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnyWhere, Category = "Combat")
	bool bAutomatic;

	//Zoom FOW while aming

	UPROPERTY(EditAnyWhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnyWhere)
	float ZoomedInterpSpeed = 30.f;

protected:
	virtual void BeginPlay() override;

	virtual void OnRep_Owner() override;


	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	UPROPERTY(VisibleAnyWhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnyWhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState ,VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(EditAnyWhere, Category = "Weapon Properties")
	class UWidgetComponent* PickUpWidget;

	UFUNCTION()
	void OnRep_WeaponState();


	UPROPERTY(EditAnyWhere, ReplicatedUsing = OnRep_WeaponState, Category = "Weapon ammo")
	int32 Ammo;

	UPROPERTY(EditAnyWhere, Category = "Weapon ammo")
	int32 MagCapacity;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();



	UPROPERTY(EditAnyWhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnyWhere, Category = "Weapon Properties")
	TSubclassOf<class ACasing> CasingClass;


	class ABlasterCharacter* BlasterOwnerCharacter;
	class ABlastPlayerController* BlasterOwnerController;


public:
	void SetWeaponState(EWeaponState weaponState);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomedInterpSpeed() const { return ZoomedInterpSpeed; }

};
