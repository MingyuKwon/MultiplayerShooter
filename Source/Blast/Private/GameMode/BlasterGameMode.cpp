// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlasterGameMode.h"
#include "character/BlasterCharacter.h"
#include "Controller/BlastPlayerController.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* eliminatedCharacter, ABlastPlayerController* killingController, ABlastPlayerController* killedController)
{
	if (killedController)
	{
		eliminatedCharacter->Eliminated();
	}
}
