// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckTargetStimuli.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API USBTService_CheckTargetStimuli : public UBTService
{
	GENERATED_BODY()

public:
	USBTService_CheckTargetStimuli();
protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LastSeenLocationKey;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector IsInSightKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActorKey;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
