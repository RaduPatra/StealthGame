// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindClosestPatrolPoint.h"

#include "BTService_CheckEscapeDistance.h"
#include "AIController.h"
#include "AISettings.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Misc/RuntimeErrors.h"

UBTTask_FindClosestPatrolPoint::UBTTask_FindClosestPatrolPoint()
{
	CurrentPatrolIndexKey.AddIntFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_FindClosestPatrolPoint, CurrentPatrolIndexKey));
}

EBTNodeResult::Type UBTTask_FindClosestPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	ClosestPointIndex = GetClosestPointIndexToPawn(ControlledPawn, PatrolPoints);

	const auto BlackBoard = OwnerComp.GetBlackboardComponent();
	BlackBoard->SetValueAsInt(CurrentPatrolIndexKey.SelectedKeyName, ClosestPointIndex);

	auto Location = PatrolPoints[ClosestPointIndex]->GetActorLocation();
	BlackBoard->SetValueAsVector(PatrolLocationKey.SelectedKeyName, Location);

	return EBTNodeResult::Succeeded;
}

int UBTTask_FindClosestPatrolPoint::GetClosestPointIndexToPawn(APawn* MyPawn, TArray<AActor*> PatrolPoints)
{
	float minDistance = BIG_NUMBER;
	float index = 0;

	for (auto i = 0; i < PatrolPoints.Num(); i++)
	{
		float distToPoint = FVector::Distance(PatrolPoints[i]->GetActorLocation(), MyPawn->GetActorLocation());
		if (distToPoint < minDistance)
		{
			minDistance = distToPoint;
			index = i;
		}
	}
	return index;
}


