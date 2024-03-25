// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */

// custom matchstate
namespace MatchState
{
	extern BLAST_API const FName CoolDown;
}

UCLASS()
class BLAST_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABlasterGameMode();

	virtual void PlayerEliminated(class ABlasterCharacter* eliminatedCharacter, class ABlastPlayerController* killingController, ABlastPlayerController* killedController);

	virtual void RequestRespawn(class ACharacter* ElimCharacter, class AController* ElimController);

	UPROPERTY(EditAnywhere)
	float WarmupTime = 10.f;

	UPROPERTY(EditAnywhere)
	float CooldownTime = 10.f;


	UPROPERTY(EditAnywhere)
	float MatchTime = 120.f;

	float LevelStartTime = 0.f;


protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;

private:

	float CountdownTime = 10.f;
};
