#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied,
	ECS_Reloading,

	ECS_MAX
};