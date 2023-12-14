// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SGDashAnimationData.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API USGDashAnimationData : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DashMontage;

	UPROPERTY(EditDefaultsOnly)
	float DashStartDistance;

	UPROPERTY(EditDefaultsOnly)
	float DashEndDistance;

};
