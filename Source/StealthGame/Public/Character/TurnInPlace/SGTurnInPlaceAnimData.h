// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SGTurnInPlaceAnimData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FTurnAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* TurnRight90Animation;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* TurnRight180Animation;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* TurnLeft90Animation;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* TurnLeft180Animation;
};


UCLASS(BlueprintType)
class STEALTHGAME_API USGTurnInPlaceAnimData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FTurnAnimationData TurnInPlaceAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RotationWeightCurveName = "TurnYawWeight";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName TurnPercentageCurveName = "TurnPercentageCurve";
};
