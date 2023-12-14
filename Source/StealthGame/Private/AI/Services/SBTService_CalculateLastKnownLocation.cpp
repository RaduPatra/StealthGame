// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/SBTService_CalculateLastKnownLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "AI/AIDefinitions.h"
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
	IsInCombatKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, IsInCombatKey));
	AlertAwarenessOnKey.AddBoolFilter(
		this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, AlertAwarenessOnKey));
	InvestigationAwarenessKey.AddBoolFilter(
		this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, InvestigationAwarenessKey));
	LastKnownLocationKey.AddVectorFilter(
		this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, LastKnownLocationKey));
	TargetActorKey.AddObjectFilter(
		this, GET_MEMBER_NAME_CHECKED(USBTService_CalculateLastKnownLocation, TargetActorKey), AActor::StaticClass());

	bNotifyBecomeRelevant = true;
}

void USBTService_CalculateLastKnownLocation::CalculateSenses(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackBoardComp)) return;

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController)) return;

	const APawn* AIPawn = MyController->GetPawn();
	if (!ensure(AIPawn)) return;

	AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		BlackBoardComp->SetValueAsBool(InvestigationAwarenessKey.SelectedKeyName, false);
		return;
	}

	FVector LastKnownLocation = BlackBoardComp->GetValueAsVector(LastKnownLocationKey.SelectedKeyName);
	const auto CurrentState = BlackBoardComp->GetValueAsEnum(CurrentDetectionStateKey.SelectedKeyName);
	const float DistanceToActor = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

	//if close to ai and alert (no matter if seen or not) he will sense you and last known location is your location
	const bool bShouldSenseAlert =
		CurrentState == static_cast<uint8>(EDetectionState::Alert) && (DistanceToActor < NoEscapeRange || BlackBoardComp
			->GetValueAsBool(IsInCombatKey.SelectedKeyName));

	BlackBoardComp->SetValueAsBool(AlertAwarenessOnKey.SelectedKeyName, bShouldSenseAlert);

	if (bShouldSenseAlert)
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
			const bool bIsInSight = Stimuli.WasSuccessfullySensed();
			BlackBoardComp->SetValueAsBool(InvestigationAwarenessKey.SelectedKeyName, bIsInSight);
			if (Stimuli.WasSuccessfullySensed())
			{
				LastKnownLocation = Stimuli.StimulusLocation;
			}
			else
			{
				int k = 1; 
			}
		}
	}
	BlackBoardComp->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, LastKnownLocation);
}

void USBTService_CalculateLastKnownLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                      float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	CalculateSenses(OwnerComp);
}

void USBTService_CalculateLastKnownLocation::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	CalculateSenses(OwnerComp);
}

void USBTService_CalculateLastKnownLocation::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}
