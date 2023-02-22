// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Testing.h"

#include "BTService_CheckEscapeDistance.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Misc/RuntimeErrors.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeManager.h"


UBTTask_Testing::UBTTask_Testing()
{
	bNotifyTick = 1;
}

EBTNodeResult::Type UBTTask_Testing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ControllerTest = OwnerComp.GetAIOwner();
	auto name = ControllerTest->GetName();
	UE_LOG(LogTemp, Warning, TEXT("BTName: %s"), *name);


	return EBTNodeResult::InProgress;
}

void UBTTask_Testing::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	auto name = ControllerTest->GetName();
	UE_LOG(LogTemp, Warning, TEXT("ctrl: %s"), *name);
}
