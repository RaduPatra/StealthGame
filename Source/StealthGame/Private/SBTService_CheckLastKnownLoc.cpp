// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTService_CheckLastKnownLoc.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Perception/AIPerceptionComponent.h"

class UAISense_Sight;

USBTService_CheckLastKnownLoc::USBTService_CheckLastKnownLoc()
{
	LocationKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CheckLastKnownLoc, LocationKey),
	                            AActor::StaticClass());
	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CheckLastKnownLoc, LocationKey));
}

void USBTService_CheckLastKnownLoc::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackBoardComp))
	{
		if (LocationKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(LocationKey.SelectedKeyName));
			if (TargetActor)
			{
				BlackBoardComp->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, TargetActor->GetActorLocation());
			}
		}
		else if (LocationKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			FVector LastSeenLocation = BlackBoardComp->GetValueAsVector(LocationKey.SelectedKeyName);
			BlackBoardComp->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, LastSeenLocation);
		}
	}
}

void USBTService_CheckLastKnownLoc::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	UBlackboardData* BBAsset = GetBlackboardAsset();

	//you need to resolve key to make SelectedKeyType available...
	if (BBAsset) LocationKey.ResolveSelectedKey(*BBAsset);
}
