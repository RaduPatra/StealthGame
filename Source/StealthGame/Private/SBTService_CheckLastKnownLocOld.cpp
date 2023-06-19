// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTService_CheckLastKnownLocOld.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Perception/AIPerceptionComponent.h"

class UAISense_Sight;

USBTService_CheckLastKnownLocOld::USBTService_CheckLastKnownLocOld()
{
	LocationKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CheckLastKnownLocOld, LocationKey),
	                            AActor::StaticClass());
	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CheckLastKnownLocOld, LocationKey));
}

void USBTService_CheckLastKnownLocOld::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

void USBTService_CheckLastKnownLocOld::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	UBlackboardData* BBAsset = GetBlackboardAsset();

	//you need to resolve key to make SelectedKeyType available...
	if (BBAsset) LocationKey.ResolveSelectedKey(*BBAsset);
}
