// Fill out your copyright notice in the Description page of Project Settings.


#include "character/BlasterCharacter.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Controller/BlastPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		Controller = Controller == nullptr ? Cast<ABlastPlayerController>(BlasterCharacter->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetScoreHUD(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		Controller = Controller == nullptr ? Cast<ABlastPlayerController>(BlasterCharacter->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetDefeatHUD(Defeats);
		}
	}
}

void ABlasterPlayerState::AddtoScore(float scoreAmout)
{
	SetScore(GetScore() + scoreAmout);

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		Controller = Controller == nullptr ? Cast<ABlastPlayerController>(BlasterCharacter->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetScoreHUD(GetScore());
		}
	}
}

void ABlasterPlayerState::AddtoDefeat(int32 DefeatsAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("AddtoDefeat"));

	Defeats += DefeatsAmount;

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		Controller = Controller == nullptr ? Cast<ABlastPlayerController>(BlasterCharacter->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetDefeatHUD(Defeats);
		}
	}
}
