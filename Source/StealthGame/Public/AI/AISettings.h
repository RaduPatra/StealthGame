// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AISettings.generated.h"


class USGAIInteractableBehaviorData;

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEALTHGAME_API UAISettings : public UActorComponent
{
	GENERATED_BODY()

public:
	UAISettings();
	virtual void BeginPlay() override;
	TArray<AActor*> GetPatrolPoints() const { return PatrolPoints; }
	USGAIInteractableBehaviorData* GetInteractableBehaviorData() const { return InteractableBehaviorData; }

protected:
	UPROPERTY(EditAnywhere)
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere)
	USGAIInteractableBehaviorData* InteractableBehaviorData;
};
