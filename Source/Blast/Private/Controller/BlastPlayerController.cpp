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
