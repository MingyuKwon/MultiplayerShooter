#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8 
{
	ERIP_Left ,
	ERIP_Right,
	ERIP_NotTurning,

	ERIP_Max
};