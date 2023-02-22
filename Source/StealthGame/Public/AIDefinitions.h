#pragma once

UENUM(BlueprintType)
enum class EDetectionState : uint8
{
	Neutral,
	Suspicious,
	Investigating,
	Alert
	
};
