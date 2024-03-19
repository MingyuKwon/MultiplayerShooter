// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlasterGameMode.h"
#include "character/BlasterCharacter.h"
#include "Controller/BlastPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerState/BlasterPlayerState.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* eliminatedCharacter, ABlastPlayerController* killingController, ABlastPlayerController* killedController)
{
	ABlasterPlayerState* KillerPlayerstate = killingController ? Cast<ABlasterPlayerState>(killingController->PlayerState) : nullptr;
	ABlasterPlayerState* KilledPlayerstate = killedController ? Cast<ABlasterPlayerState>(killedController->PlayerState) : nullptr;

	if (KillerPlayerstate && KillerPlayerstate != KilledPlayerstate)
	{
		KillerPlayerstate->AddtoScore(1.f);
	}

	if (killedController)
	{
		eliminatedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimCharacter, AController* ElimController)
{
	if (ElimCharacter)
	{
		ElimCharacter->Reset();
		ElimCharacter->Destroy();
	}

	if (ElimController)
	{
		TArray<AActor*> actorArray;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), actorArray);
		int32 Selection = FMath::RandRange(0, actorArray.Num() - 1);

		RestartPlayerAtPlayerStart(ElimController, actorArray[Selection]);
	}
}
