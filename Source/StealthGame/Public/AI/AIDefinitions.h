#pragma once

UENUM(BlueprintType)
enum class EDetectionState : uint8
{
	Neutral,
	Searching,
	Investigating,
	Alert
};


UENUM(BlueprintType)
enum class EAIState : uint8
{
	Passive,
	Investigating,
	Alert
};
