// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckLastKnownLoc.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API USBTService_CheckLastKnownLoc : public UBTService
{
	GENERATED_BODY()
public:
	USBTService_CheckLastKnownLoc();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LastKnownLocationKey;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LocationKey;
};
