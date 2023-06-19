#pragma once

UENUM(BlueprintType)
enum class EDetectionState : uint8
{
	Neutral,
	Searching,
	Investigating,
	Alert
};
