// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetNextPatrolLocation.h"

#include "AISettings.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetNextPatrolLocation::UBTTask_GetNextPatrolLocation()
{
}

EBTNodeResult::Type UBTTask_GetNextPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	auto ControlledPawn = MyController->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	UAISettings* AISettings = ControlledPawn->FindComponentByClass<UAISettings>();
	if (!AISettings) return EBTNodeResult::Failed;

	auto PatrolPoints = AISettings->GetPatrolPoints();
	if (!PatrolPoints.Num()) return EBTNodeResult::Failed;

	const auto BlackBoard = OwnerComp.GetBlackboardComponent();
	auto PatrolIndex = BlackBoard->GetValueAsInt(CurrentPatrolIndexKey.SelectedKeyName);
	PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	auto PatrolLocation = PatrolPoints[PatrolIndex]->GetActorLocation();

	BlackBoard->SetValueAsInt(CurrentPatrolIndexKey.SelectedKeyName, PatrolIndex);
	BlackBoard->SetValueAsVector(PatrolLocationKey.SelectedKeyName, PatrolLocation);
	return EBTNodeResult::Succeeded;
}
