// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_FindClosestPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API UBTTask_FindClosestPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_FindClosestPatrolPoint();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CurrentPatrolIndexKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolLocationKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PatrolRotationKey;


	UPROPERTY()
	int ClosestPointIndex = 0;

private:
	static int GetClosestPointIndexToPawn(APawn* MyPawn, TArray<AActor*> PatrolPoints);
};
