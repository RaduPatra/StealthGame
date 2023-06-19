// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckLastKnownLocOld.generated.h"

/**
 *
 * This used to update the last known location before moving the logic to a more specific node that also handles the update conditions.
 * Previously we had to make separate branches for the conditions which was a bit messy since that shouldn't have been the tree responsability
 */

UCLASS()
class STEALTHGAME_API USBTService_CheckLastKnownLocOld : public UBTService
{
	GENERATED_BODY()
public:
	USBTService_CheckLastKnownLocOld();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LastKnownLocationKey;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LocationKey;

};
