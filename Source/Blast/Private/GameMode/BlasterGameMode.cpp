// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlasterGameMode.h"
#include "character/BlasterCharacter.h"
#include "Controller/BlastPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerState/BlasterPlayerState.h"
#include "GameState/BlasterGameState.h"

namespace MatchState
{
	const FName CoolDown = FName("CoolDown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartTime + MatchTime;
		if (CountdownTime < 0)
		{
			SetMatchState(MatchState::CoolDown);
		}
	}
	else if (MatchState == MatchState::CoolDown)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartTime + MatchTime + CooldownTime;
		if (CountdownTime < 0)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator IT = GetWorld()->GetPlayerControllerIterator(); IT; IT++)
	{
		ABlastPlayerController* playerController = Cast<ABlastPlayerController>(*IT);
		if (playerController)
		{
			playerController->OnMatchStateSet(MatchState);
		}
	}
\
	
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* eliminatedCharacter, ABlastPlayerController* killingController, ABlastPlayerController* killedController)
{
	if (killingController == nullptr || killingController->PlayerState == nullptr) return;
	if (killedController == nullptr || killedController->PlayerState == nullptr) return;

	ABlasterPlayerState* KillerPlayerstate = killingController ? Cast<ABlasterPlayerState>(killingController->PlayerState) : nullptr;
	ABlasterPlayerState* KilledPlayerstate = killedController ? Cast<ABlasterPlayerState>(killedController->PlayerState) : nullptr;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if (KillerPlayerstate && KillerPlayerstate != KilledPlayerstate && BlasterGameState)
	{
		KillerPlayerstate->AddtoScore(1.f);
		BlasterGameState->UpdateTopScore(KillerPlayerstate);
	}

	if (KilledPlayerstate)
	{
		KilledPlayerstate->AddtoDefeat(1);
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


