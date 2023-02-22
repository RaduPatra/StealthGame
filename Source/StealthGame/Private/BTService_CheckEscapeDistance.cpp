// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckEscapeDistance.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"


UBTService_CheckEscapeDistance::UBTService_CheckEscapeDistance()
{

}

void UBTService_CheckEscapeDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if (TargetActor)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();
			APawn* AIPawn = MyController->GetPawn();

			//update distance
			if (ensure(AIPawn))
			{
				float DistanceToActor = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
				BlackBoardComp->SetValueAsBool(InEscapeRangeKey.SelectedKeyName, DistanceToActor > NoEscapeRange);
			}
		}
	}
}

