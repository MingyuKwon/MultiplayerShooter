// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/BlastPlayerController.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "character/BlasterCharacter.h"

void ABlastPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
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
	uint32 timeLeft = FMath::CeilToInt(MatcthTime - GetServerTime());

	if (timeLeft != countDownInt)
	{
		SetMatchTimeHUD(timeLeft);
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
