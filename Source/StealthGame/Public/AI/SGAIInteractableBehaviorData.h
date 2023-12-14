// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SGAIInteractableBehaviorData.generated.h"

/**
 * 
 */

class UBehaviorTree;

UCLASS()
class STEALTHGAME_API USGAIInteractableBehaviorData : public UDataAsset
{
	GENERATED_BODY()

public:
	const TMap<FGameplayTag, UBehaviorTree*>& GetInteractableBehaviorMap() const { return InteractableBehaviorMap; }

protected:
	UPROPERTY(EditAnywhere, Category = "Behavior")
	TMap<FGameplayTag, UBehaviorTree*> InteractableBehaviorMap;
};
