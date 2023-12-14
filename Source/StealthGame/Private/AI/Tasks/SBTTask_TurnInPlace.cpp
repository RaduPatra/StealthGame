// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SBTTask_TurnInPlace.h"

#include "AI/NPC_AIController.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

USBTTask_TurnInPlace::USBTTask_TurnInPlace()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	TurnLocationKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USBTTask_TurnInPlace, TurnLocationKey),
	                                AActor::StaticClass());
	TurnLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(USBTTask_TurnInPlace, TurnLocationKey));
	TurnLocationKey.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(USBTTask_TurnInPlace, TurnLocationKey));
}

EBTNodeResult::Type USBTTask_TurnInPlace::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) return EBTNodeResult::Failed;

	const APawn* MyPawn = MyController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	TurnInPlaceComponent = MyPawn->FindComponentByClass<UTurnInPlaceComponent>();
	if (!TurnInPlaceComponent) return EBTNodeResult::Failed;
	MyOwnerComp = &OwnerComp;

	const auto BlackBoard = OwnerComp.GetBlackboardComponent();
	const FVector AILocation = MyController->GetPawn()->GetActorLocation();
	auto Rotation = GetTurnRotation(BlackBoard, AILocation);
	if (!Rotation.IsSet())
	{
		return EBTNodeResult::Failed;
	}
	
	TurnInPlaceComponent->OnTurnInPlaceFinished.AddDynamic(this, &USBTTask_TurnInPlace::TurnEnded);
	TurnInPlaceComponent->StartTurnInPlace(Rotation.GetValue());
	return EBTNodeResult::InProgress;
}

void USBTTask_TurnInPlace::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type USBTTask_TurnInPlace::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!IsValid(TurnInPlaceComponent)) return EBTNodeResult::Aborted;
	TurnInPlaceComponent->InterruptTurnInPlace();
	return EBTNodeResult::Aborted;
}

void USBTTask_TurnInPlace::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                          EBTNodeResult::Type TaskResult)
{
	TurnInPlaceComponent->OnTurnInPlaceFinished.RemoveDynamic(this, &USBTTask_TurnInPlace::TurnEnded);
}

uint16 USBTTask_TurnInPlace::GetInstanceMemorySize() const
{
	return Super::GetInstanceMemorySize();
}

void USBTTask_TurnInPlace::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TurnLocationKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning,
		       TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}


TOptional<FRotator> USBTTask_TurnInPlace::GetTurnRotation(UBlackboardComponent* BlackBoard,
                                                          const FVector& AILocation) const
{
	FVector TurnLocation;
	if (TurnLocationKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TurnLocation = BlackBoard->GetValueAsVector(TurnLocationKey.SelectedKeyName);
	}
	else if (TurnLocationKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		const AActor* TurnTarget = Cast<AActor>(BlackBoard->GetValueAsObject(TurnLocationKey.SelectedKeyName));
		if (IsValid(TurnTarget))
		{
			TurnLocation = TurnTarget->GetActorLocation();
		}
		else
		{
			return TOptional<FRotator>();
		}
	}
	else if (TurnLocationKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	{
		FRotator TurnRotation = BlackBoard->GetValueAsRotator(TurnLocationKey.SelectedKeyName);
		return TurnRotation;
	}

	if (TurnLocation.IsZero() || !FAISystem::IsValidLocation(TurnLocation))
	{
		return TOptional<FRotator>();
	}

	return (TurnLocation - AILocation).GetSafeNormal().Rotation();
}

void USBTTask_TurnInPlace::TurnEnded(bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("TaskCompleted"));
	FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
}
