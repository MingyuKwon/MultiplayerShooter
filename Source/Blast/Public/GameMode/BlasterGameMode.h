// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
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

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

private:

	float LevelStartTime = 0.f;
	float CountdownTime = 10.f;
};
