// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_GetNextPatrolLocation.h"

#include "AI/AISettings.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetNextPatrolLocation::UBTTask_GetNextPatrolLocation()
{
	CurrentPatrolIndexKey.AddIntFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_GetNextPatrolLocation, CurrentPatrolIndexKey));
	PatrolLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetNextPatrolLocation, PatrolLocationKey));
	PatrolRotationKey.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetNextPatrolLocation, PatrolRotationKey));
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
	int PatrolIndex = BlackBoard->GetValueAsInt(CurrentPatrolIndexKey.SelectedKeyName);
	PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	const FVector PatrolLocation = PatrolPoints[PatrolIndex]->GetActorLocation();
	FRotator PatrolRotation = PatrolPoints[PatrolIndex]->GetActorRotation();
	

	BlackBoard->SetValueAsInt(CurrentPatrolIndexKey.SelectedKeyName, PatrolIndex);
	BlackBoard->SetValueAsVector(PatrolLocationKey.SelectedKeyName, PatrolLocation);
	BlackBoard->SetValueAsRotator(PatrolRotationKey.SelectedKeyName, PatrolRotation);


	return EBTNodeResult::Succeeded;
}
