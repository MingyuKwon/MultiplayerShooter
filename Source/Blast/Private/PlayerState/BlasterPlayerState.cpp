// Fill out your copyright notice in the Description page of Project Settings.


#include "character/BlasterCharacter.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Controller/BlastPlayerController.h"

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		Controller = Controller == nullptr ? Cast<ABlastPlayerController>(BlasterCharacter->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetScoreHUD(Score);
		}
	}
}

void ABlasterPlayerState::AddtoScore(float scoreAmout)
{
	Score += scoreAmout;

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		Controller = Controller == nullptr ? Cast<ABlastPlayerController>(BlasterCharacter->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetScoreHUD(Score);
		}
	}
}
