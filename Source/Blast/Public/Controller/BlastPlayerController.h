// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlastPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLAST_API ABlastPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHealthHUD(float Health, float MaxHealth);
	void SetScoreHUD(float Score);
	void SetDefeatHUD(int32 Defeats);


	void SetEquipAmmoHUD(int32 Ammo);
	void SetCarriedAmmoHUD(int32 Ammo);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* pawn) override;


private:
	class ABlasterHUD* BlasterHUD;
};
