// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLAST_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();


	void AddtoScore(float scoreAmout);
	void AddtoDefeat(int32 DefeatsAmount);

private:
	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter;
	UPROPERTY()
	class ABlastPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

};