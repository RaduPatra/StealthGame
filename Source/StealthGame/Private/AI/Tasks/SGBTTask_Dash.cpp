// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SGBTTask_Dash.h"

USGBTTask_Dash::USGBTTask_Dash()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type USGBTTask_Dash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void USGBTTask_Dash::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type USGBTTask_Dash::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void USGBTTask_Dash::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
