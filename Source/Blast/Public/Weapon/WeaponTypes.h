#pragma once
#define TRACE_LENGTH 80000

UENUM(BlueprintType)
enum class EWeaponType
{
	EWT_AssaultRifle,
	EWT_RocketLauncher,
	EWT_Pistol,
	EWT_SubMachinegun,
	EWT_Shotgun,
	EWT_SniperRifle,
	EWT_GrenadeLauncer,

	EWT_MAX
};