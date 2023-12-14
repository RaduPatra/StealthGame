// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FindClosestPatrolPoint.h"

#include "AI/Services/BTService_CheckEscapeDistance.h"
#include "AIController.h"
#include "AI/AISettings.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"

UBTTask_FindClosestPatrolPoint::UBTTask_FindClosestPatrolPoint()
{
	CurrentPatrolIndexKey.AddIntFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_FindClosestPatrolPoint, CurrentPatrolIndexKey));
	PatrolLocationKey.AddVectorFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_FindClosestPatrolPoint, PatrolLocationKey));
	PatrolRotationKey.AddRotatorFilter(
		this, GET_MEMBER_NAME_CHECKED(UBTTask_FindClosestPatrolPoint, PatrolRotationKey));
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

	const FVector Location = PatrolPoints[ClosestPointIndex]->GetActorLocation();
	const FRotator Rotation = PatrolPoints[ClosestPointIndex]->GetActorRotation();
	BlackBoard->SetValueAsVector(PatrolLocationKey.SelectedKeyName, Location);
	BlackBoard->SetValueAsRotator(PatrolRotationKey.SelectedKeyName, Rotation);

	return EBTNodeResult::Succeeded;
}

int UBTTask_FindClosestPatrolPoint::GetClosestPointIndexToPawn(APawn* MyPawn, TArray<AActor*> PatrolPoints)
{
	float MinDistance = BIG_NUMBER;
	float Index = 0;

	for (auto i = 0; i < PatrolPoints.Num(); i++)
	{
		const float DistToPoint = FVector::Distance(PatrolPoints[i]->GetActorLocation(), MyPawn->GetActorLocation());
		if (DistToPoint < MinDistance)
		{
			MinDistance = DistToPoint;
			Index = i;
		}
	}
	return Index;
}
