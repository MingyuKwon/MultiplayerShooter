// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/BlastPlayerController.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/BlasterGameMode.h"
#include "HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"

void ABlastPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

	ServerCheckMatchState();

	
}

void ABlastPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlastPlayerController, MatchState);
}

void ABlastPlayerController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(pawn);
	if (BlasterCharacter)
	{
		SetHealthHUD(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
	
}

void ABlastPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHudTime();

	CheckTimeSync(DeltaTime);
	PollInit();
}



void ABlastPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequenty)
	{
		TimeSyncRunningTime = 0.f;
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlastPlayerController::SetHudTime()
{
	float LeftTime = 0.f;

	if (MatchState == MatchState::WaitingToStart)
	{
		LeftTime = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		LeftTime = WarmupTime - GetServerTime() + LevelStartingTime + MatcthTime;
	}

	uint32 timeLeft = FMath::CeilToInt(LeftTime);

	if (timeLeft != countDownInt)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountDown(timeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetMatchTimeHUD(timeLeft);
		}
	}

	countDownInt = timeLeft;

}



float ABlastPlayerController::GetServerTime()
{
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlastPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}



void ABlastPlayerController::ServerRequestServerTime_Implementation(float TimeofClientRequest)
{
	float ServerTime = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeofClientRequest, ServerTime);
}

void ABlastPlayerController::ClientReportServerTime_Implementation(float TimeofClientRequest, float TimeServerReceiveClientRequest)
{
	float ClientTime = GetWorld()->GetTimeSeconds();
	float RTT = ClientTime - TimeofClientRequest;
	float CurrentServerTime = TimeServerReceiveClientRequest + RTT * 0.5;

	ClientServerDelta = CurrentServerTime - ClientTime;
}

void ABlastPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		MatcthTime = GameMode->MatchTime;
		WarmupTime = GameMode->WarmupTime;
		LevelStartingTime = GameMode->LevelStartTime;

		MatchState = GameMode->GetMatchState();

		ClientJoinMidGame(MatchState, MatcthTime, WarmupTime, LevelStartingTime);
	}
}

void ABlastPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float match, float warmup, float levelstarting)
{
	MatcthTime = match;
	WarmupTime = warmup;
	LevelStartingTime = levelstarting;

	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		UE_LOG(LogTemp, Warning, TEXT("In ClientJoinMidGame_Implementation"))

		BlasterHUD->AddAnnouncement();
	}
}


void ABlastPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->OverlayWidget)
		{
			CharacterOverlay = BlasterHUD->OverlayWidget;

			if (CharacterOverlay)
			{
				SetHealthHUD(HUDHealth, HUDMaxHealth);
				SetScoreHUD(HUDScore);
				SetDefeatHUD(HUDDefeat);
			}
		}
	}
}

void ABlastPlayerController::SetHealthHUD(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bAvailable = BlasterHUD 
		&& BlasterHUD->OverlayWidget 
		&& BlasterHUD->OverlayWidget->HealthBar 
		&& BlasterHUD->OverlayWidget->HealthText;

	if (bAvailable)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->OverlayWidget->HealthBar->SetPercent(HealthPercent);

		FString healthTEXT = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt32(Health) , FMath::CeilToInt32(MaxHealth));
		BlasterHUD->OverlayWidget->HealthText->SetText(FText::FromString(healthTEXT));

	}
	else
	{
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlastPlayerController::SetScoreHUD(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bAvailable = BlasterHUD
		&& BlasterHUD->OverlayWidget
		&& BlasterHUD->OverlayWidget->ScoreAmount;

	if (bAvailable)
	{
		FString ScoreTEXT = FString::Printf(TEXT("%d"), FMath::CeilToInt32(Score));
		BlasterHUD->OverlayWidget->ScoreAmount->SetText(FText::FromString(ScoreTEXT));
	}
	else
	{
		HUDScore = Score;
	}

}

void ABlastPlayerController::SetDefeatHUD(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bAvailable = BlasterHUD
		&& BlasterHUD->OverlayWidget
		&& BlasterHUD->OverlayWidget->DefeatAmount;

	if (bAvailable)
	{
		FString DefeatTEXT = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->OverlayWidget->DefeatAmount->SetText(FText::FromString(DefeatTEXT));
	}
	else
	{
		HUDDefeat = Defeats;
	}
}

void ABlastPlayerController::SetMatchTimeHUD(float CountDownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bAvailable = BlasterHUD
		&& BlasterHUD->OverlayWidget
		&& BlasterHUD->OverlayWidget->MatchCountdownText;

	if (bAvailable)
	{

		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60.f;
		FString TimeTEXT = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->OverlayWidget->MatchCountdownText->SetText(FText::FromString(TimeTEXT));
	}

}

void ABlastPlayerController::SetHUDAnnouncementCountDown(float CountDownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bAvailable = BlasterHUD
		&& BlasterHUD->AnnouncementWidget
		&& BlasterHUD->AnnouncementWidget->WarmUpTime;

	if (bAvailable)
	{

		int32 Minutes = FMath::FloorToInt(CountDownTime / 60.f);
		int32 Seconds = CountDownTime - Minutes * 60.f;
		FString TimeTEXT = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->AnnouncementWidget->WarmUpTime->SetText(FText::FromString(TimeTEXT));
	}

}

void ABlastPlayerController::SetEquipAmmoHUD(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bAvailable = BlasterHUD
		&& BlasterHUD->OverlayWidget
		&& BlasterHUD->OverlayWidget->WeaponAmmoAmount;

	if (bAvailable)
	{
		FString AmmoTEXT = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->OverlayWidget->WeaponAmmoAmount->SetText(FText::FromString(AmmoTEXT));

	}

}

void ABlastPlayerController::SetCarriedAmmoHUD(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bAvailable = BlasterHUD
		&& BlasterHUD->OverlayWidget
		&& BlasterHUD->OverlayWidget->CarriedAmmoAmount;

	if (bAvailable)
	{
		FString AmmoTEXT = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->OverlayWidget->CarriedAmmoAmount->SetText(FText::FromString(AmmoTEXT));

	}
}

void ABlastPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	// 여기서 하기에는 waitingstart 차례에는 hud가 존재 하지 않아서 add 하려고 해도 무조건 튕긴다

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchStart();
	}
	else if (MatchState == MatchState::CoolDown)
	{
		HandleCooldown();
	}
}

void ABlastPlayerController::OnRep_MatchState()
{

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchStart();
	}
	else if (MatchState == MatchState::CoolDown)
	{
		HandleCooldown();
	}
}

void ABlastPlayerController::HandleMatchStart()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->AddCharacterOverlay();

		if (BlasterHUD->AnnouncementWidget)
		{
			BlasterHUD->AnnouncementWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlastPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->AnnouncementWidget)
		{
			BlasterHUD->AnnouncementWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
