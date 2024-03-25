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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void SetHealthHUD(float Health, float MaxHealth);
	void SetScoreHUD(float Score);
	void SetDefeatHUD(int32 Defeats);
	void SetMatchTimeHUD(float CountDownTime);


	void SetEquipAmmoHUD(int32 Ammo);
	void SetCarriedAmmoHUD(int32 Ammo);

	void OnMatchStateSet(FName State);



protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* pawn) override;
	virtual void Tick(float DeltaTime) override;

	void PollInit();


	void SetHudTime();

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeofClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeofClientRequest, float TimeServerReceiveClientRequest);

	float ClientServerDelta = 0.f;

	UPROPERTY(EditAnyWhere, Category = "Time")
	float TimeSyncFrequenty = 5.f;
	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;


private:
	class ABlasterHUD* BlasterHUD;

	float MatcthTime = 120.f;
	uint32 countDownInt = 0;

	UPROPERTY(EditAnyWhere, ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();


	UPROPERTY()
	class UCharacterOverlayWidget* CharacterOverlay;

	bool InitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	float HUDDefeat;
};
