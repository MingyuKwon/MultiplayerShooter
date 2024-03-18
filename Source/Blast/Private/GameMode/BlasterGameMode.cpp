// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlasterGameMode.h"
#include "character/BlasterCharacter.h"
#include "Controller/BlastPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* eliminatedCharacter, ABlastPlayerController* killingController, ABlastPlayerController* killedController)
{
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
