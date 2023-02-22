// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTService_CheckTargetStimuli.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Misc/RuntimeErrors.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"


USBTService_CheckTargetStimuli::USBTService_CheckTargetStimuli()
{
}

void USBTService_CheckTargetStimuli::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if (TargetActor)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();
			FActorPerceptionBlueprintInfo Info;
			MyController->GetPerceptionComponent()->GetActorsPerception(TargetActor, Info); //todo null check perception
			for (auto Stimuli : Info.LastSensedStimuli)
			{
				if (Stimuli.Type == UAISense::GetSenseID<UAISense_Sight>())
				{
					BlackBoardComp->SetValueAsVector(LastSeenLocationKey.SelectedKeyName, Stimuli.StimulusLocation);
					BlackBoardComp->SetValueAsBool(IsInSightKey.SelectedKeyName, Stimuli.WasSuccessfullySensed());
				}
			}
		}
	}
}
