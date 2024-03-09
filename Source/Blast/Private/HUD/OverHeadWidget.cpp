// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverHeadWidget.h"
#include "GameFramework/PlayerState.h"
#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisPlayText(FString TextToDisaplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisaplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole localRole = InPawn->GetLocalRole();
	FString Role;

	switch(localRole)
	{
	case ENetRole::ROLE_Authority :
		Role = FString("Authority");
		break;

	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;

	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;

	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}

	FString LocalRoleString = FString::Printf(TEXT("Local Role : %s"), *Role);
	// PlayerName = InPawn->GetPlayerState()->GetPlayerName();
	SetDisPlayText(LocalRoleString);
	//SetDisPlayText(PlayerName);
}

void UOverHeadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
