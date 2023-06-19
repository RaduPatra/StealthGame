// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTService_CalculateLastKnownLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "AIDefinitions.h"
#include "Misc/RuntimeErrors.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"


USBTService_CalculateLastKnownLocation::USBTService_CalculateLastKnownLocation()
{
	CurrentDetectionStateKey.AddEnumFilter(
		this,
		GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, CurrentDetectionStateKey),
		StaticEnum<EDetectionState>()
	);
	IsInSightKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, IsInSightKey));
	AlertAwarenessOnKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, AlertAwarenessOnKey));
	LastKnownLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, LastKnownLocationKey));
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, TargetActorKey), AActor::StaticClass());

}

void USBTService_CalculateLastKnownLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackBoardComp)) return;

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController)) return;

	const APawn* AIPawn = MyController->GetPawn();
	if (!ensure(AIPawn)) return;

	AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return;

	auto LastKnownLocation = BlackBoardComp->GetValueAsVector(LastKnownLocationKey.SelectedKeyName);
	const auto CurrentState = BlackBoardComp->GetValueAsEnum(CurrentDetectionStateKey.SelectedKeyName);
	const float DistanceToActor = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

	//if close to ai and alert (no matter if seen or not) he will sense you and last known location is your location
	//todo? - maybe add a list of allowed states to check against instead of just alert for more configuration
	const bool bShouldSense = DistanceToActor < NoEscapeRange && CurrentState == static_cast<uint8>(
		EDetectionState::Alert);
	BlackBoardComp->SetValueAsBool(AlertAwarenessOnKey.SelectedKeyName, bShouldSense);
	if (bShouldSense)
	{
		LastKnownLocation = TargetActor->GetActorLocation();
	}
	//if seen, last known location is last seen location
	FActorPerceptionBlueprintInfo Info;
	MyController->GetPerceptionComponent()->GetActorsPerception(TargetActor, Info);

	for (auto Stimuli : Info.LastSensedStimuli)
	{
		if (Stimuli.Type == UAISense::GetSenseID<UAISense_Sight>())
		{
			// BlackBoardComp->SetValueAsVector(LastSeenLocationKey.SelectedKeyName, Stimuli.StimulusLocation);
			BlackBoardComp->SetValueAsBool(IsInSightKey.SelectedKeyName, Stimuli.WasSuccessfullySensed());
			if (Stimuli.WasSuccessfullySensed())
			{
				LastKnownLocation = Stimuli.StimulusLocation;
			}
		}
	}
	BlackBoardComp->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, LastKnownLocation);
}
