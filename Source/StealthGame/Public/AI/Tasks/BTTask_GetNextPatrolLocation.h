// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetNextPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API UBTTask_GetNextPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_GetNextPatrolLocation();
protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CurrentPatrolIndexKey;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolLocationKey;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolRotationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
