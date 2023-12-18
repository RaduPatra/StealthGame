// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/AI/AIManagerSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Game/SGBlueprintFunctionLibrary.h"
#include "AI/BBKeys.h"
#include "AI/NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

class UAISense_Hearing;;

void UAIManagerSubsystem::Tick(float DeltaTime)
{
	AssignNoiseStimuli();
}

void UAIManagerSubsystem::RegisterNPC(ANPC_AIController* Controller)
{
	NPCControllers.Add(Controller);
}

void UAIManagerSubsystem::RegisterNoiseAwareNPC(ANPC_AIController* Controller)
{
	NoiseAwareNPCs.Add(Controller);
}

void UAIManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	check(NavSys != nullptr);
}

void UAIManagerSubsystem::AssignNoiseStimuli()
{
	if (!NoiseAwareNPCs.Num()) return;

	float ClosestDistance = FLT_MAX;
	ANPC_AIController* ClosestNPC = nullptr;
	const AActor* SensedTarget = TargetStimuli.begin().Key();
	const FAIStimulus TargetStimulus = TargetStimuli.begin().Value();

	for (const auto NPC : NoiseAwareNPCs)
	{
		const AActor* Target = Cast<AActor>(NPC->GetBlackboardComponent()->GetValueAsObject(BBKeys::TargetActorKey));
		if (IsValid(Target))
		{
			continue;
		}
		double PathCost = BIG_NUMBER;
		const ENavigationQueryResult::Type Result = NavSys->GetPathLength(NPC->GetPawn()->GetActorLocation(),
		                                        TargetStimulus.StimulusLocation,
		                                        PathCost);
		if (Result != ENavigationQueryResult::Success) continue;
		if (PathCost < ClosestDistance)
		{
			ClosestDistance = PathCost;
			ClosestNPC = NPC;
		}
	}

	// Only assign the closest NPC to investigate the noise
	//in the future we could only assign the location to closest and implement a small reaction to all that heard the noise
	if (ClosestNPC)
	{
		ClosestNPC->GetBlackboardComponent()->SetValueAsBool(BBKeys::HeardNoiseKey, true);
		ClosestNPC->GetBlackboardComponent()->SetValueAsVector(BBKeys::LastSoundLocationKey,
		                                                       TargetStimulus.StimulusLocation);
		ClosestNPC->GetBlackboardComponent()->SetValueAsName(BBKeys::SoundTypeKey, TargetStimulus.Tag);

		DrawDebugSphere(GetWorld(),
		                ClosestNPC->GetPawn()->GetActorLocation(),
		                32.f,
		                12,
		                FColor::Blue,
		                false,
		                1.f);
	}

	NoiseAwareNPCs.Empty();
	TargetStimuli.Empty();
}

TStatId UAIManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAIManagerSubsystem, STATGROUP_Tickables);
}
