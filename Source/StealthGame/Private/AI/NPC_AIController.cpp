// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC_AIController.h"

// #include "AIManager.h"
#include "Character/SGCombatInterface.h"
#include "AI/BBKeys.h"
#include "AI/NPC_Character.h"
#include "Game/StealthGameGameMode.h"
#include "UI/WorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Prediction.h"
#include "Perception/AISense_Sight.h"

class UBlackboardKeyType_Bool;

ANPC_AIController::ANPC_AIController()
{
}

void ANPC_AIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::PerceptionUpdated);
}



void ANPC_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (UAIManagerSubsystem* AIManager = GetAIManager())
	{
		AIManager->RegisterNPC(this);
	}

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is Null! Don't forget to assign it!")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	if (Blackboard)
	{
		const auto KeyID = Blackboard->GetKeyID(BBKeys::TargetActorKey);
		Blackboard->RegisterObserver(KeyID, this,
		                             FOnBlackboardChangeNotification::CreateUObject(
			                             this, &ANPC_AIController::OnBlackboardKeyValueChange));
	}
}

EBlackboardNotificationResult ANPC_AIController::OnBlackboardKeyValueChange(const UBlackboardComponent& BlackboardComp,
                                                                            FBlackboard::FKey ChangedKeyID)
{
	UObject* KeyValue = BlackboardComp.GetValue<UBlackboardKeyType_Object>(ChangedKeyID);
	AActor* BBTargetActor = Cast<AActor>(KeyValue);
	if (BBTargetActor)
	{
		OnTargetAcquired(BBTargetActor);
	}
	else
	{
		OnTargetLost(TargetActor); //call for previous target
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

void ANPC_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bSuspicionDetectionEnabled)
	{
		CalculateDetectionLevel(DeltaSeconds);

		if (DetectionPercentage > 0)
		{
			CreateSuspicionWidget();
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("DetectionPercentage %f"), DetectionPercentage);
}

void ANPC_AIController::CreateSuspicionWidget()
{
	if (ActiveSuspicionMeter == nullptr && ensure(SuspicionMeterWidgetClass))
	{
		ActiveSuspicionMeter = CreateWidget<UWorldUserWidget>(GetWorld(), SuspicionMeterWidgetClass);
	}

	if (ActiveSuspicionMeter)
	{
		ActiveSuspicionMeter->AttachedActor = ControlledPawn;

		if (!ActiveSuspicionMeter->IsInViewport())
		{
			ActiveSuspicionMeter->AddToViewport();
		}
	}
}

float ANPC_AIController::CalculateSuspicionRate()
{
	const auto SenseConfig = GetPerceptionComponent()->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>());

	const auto SightSenseConfig = Cast<UAISenseConfig_Sight>(SenseConfig);

	if (!ensure(SightSenseConfig)) return 0;
	const float SightRadius = SightSenseConfig->SightRadius;

	if (!ensure(TargetActor)) return 0;

	const auto DistanceToTarget = FVector::Distance(GetPawn()->GetActorLocation(), TargetActor->GetActorLocation());
	const auto NormalizedDistance = UKismetMathLibrary::NormalizeToRange(DistanceToTarget, 0, SightRadius);

	return ControlledPawn->DetectionRateCurve->GetFloatValue(NormalizedDistance);
}

void ANPC_AIController::CalculateDetectionLevel(float DeltaSeconds)
{
	if (!ensureMsgf(ControlledPawn->DetectionRateCurve, TEXT("You need to assign the detection curve!!"))) return;
	if (bIsTargetInSight)
	{
		if (bCanSeeInstantlyDebug)
		{
			SetTargetActor(TargetActor);
			SetDetectionState(EDetectionState::Alert);
			return;
		}
		const float SuspicionRate = CalculateSuspicionRate();
		DetectionPercentage += SuspicionRate * DeltaSeconds;
		DetectionPercentage = FMath::Clamp(DetectionPercentage, 0, 1);

		if (CanTriggerInvestigation() && !IsInvestigating())
		{
			UE_LOG(LogTemp, Warning, TEXT("Started Investigating"));
			if (ensure(TargetActor))
			{
				//only set target if suspicious enough
				SetTargetActor(TargetActor);
			}

			SetDetectionState(EDetectionState::Investigating);
		}
		else if (CanTriggerAlert() && !IsAlert())
		{
			UE_LOG(LogTemp, Warning, TEXT("Started Alert"));
			SetDetectionState(EDetectionState::Alert);
		}
	}
	else
	{
		if (CanDecreaseDetection())
		{
			DetectionPercentage -= ControlledPawn->SuspicionDecreaseRate * DeltaSeconds;
			DetectionPercentage = FMath::Clamp(DetectionPercentage, 0, 1);
			SetDetectionState(EDetectionState::Neutral);
		}
	}
}


void ANPC_AIController::SetTargetActor(AActor* NewTarget)
{
	GetBlackboardComponent()->SetValueAsObject(BBKeys::TargetActorKey, NewTarget);
}


AActor* ANPC_AIController::GetTargetActor() const
{
	const APawn* MyPawn = GetPawn();
	if (IsValid(MyPawn))
	{
		return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(BBKeys::TargetActorKey));
	}

	return nullptr;
}


void ANPC_AIController::HandleSightSense(AActor* Actor, const FAIStimulus& Stimulus)
{
	if (!Actor->Implements<USGCombatInterface>()) return;
	const FGameplayTag ActorTeam = ISGCombatInterface::Execute_GetTeamTag(Actor);
	if (!DetectedTeamTags.HasTagExact(ActorTeam)) return;

	bIsTargetInSight = Stimulus.WasSuccessfullySensed();
	Blackboard->SetValueAsBool(BBKeys::TargetInSightKey, bIsTargetInSight);

	// if (IsValid(TargetActor)) return;
	if (bIsTargetInSight)
	{
		TargetActor = Actor;
	}
}

void ANPC_AIController::HandleHearingSense(AActor* Actor, const FAIStimulus& Stimulus)
{
	if (!Stimulus.IsExpired())
	{
		//register aware Npcs to manager which will calculate who can hear the noise in the next frame
		GetAIManager()->RegisterNoiseAwareNPC(this);
		GetAIManager()->SetTargetSense(Actor, Stimulus);
	}
	else
	{
		GetBlackboardComponent()->SetValueAsBool(BBKeys::HeardNoiseKey, false);
	}
}

void ANPC_AIController::PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		HandleSightSense(Actor, Stimulus);
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		HandleHearingSense(Actor, Stimulus);
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Prediction>())
	{
		
	}
}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("Posessed"));

	ANPC_Character* MyPawn = Cast<ANPC_Character>(InPawn);
	checkf(MyPawn, TEXT("ANPC_AIController possessed pawn is not valid"));
	ControlledPawn = MyPawn;
}

void ANPC_AIController::ResetSuspicion() //replaced with ResetDetectionToState
{
	DetectionPercentage = 0;

	SetDetectionState(EDetectionState::Neutral);
	GetBlackboardComponent()->SetValueAsEnum(BBKeys::DetectionStateKey, static_cast<uint8>(CurrentDetectionState));
}

void ANPC_AIController::SetDetectionState(EDetectionState DetectionState)
{
	CurrentDetectionState = DetectionState;
	if (DetectionPercentage > 0)
	{
		CreateSuspicionWidget();
	}

	GetBlackboardComponent()->SetValueAsEnum(BBKeys::DetectionStateKey, static_cast<uint8>(CurrentDetectionState));
	OnDetectionStateChange(CurrentDetectionState);
}

void ANPC_AIController::ResetDetectionToState(EDetectionState DetectionState)
{
	switch (DetectionState)
	{
	case EDetectionState::Neutral:
		DetectionPercentage = 0;
		break;
	case EDetectionState::Searching:
		DetectionPercentage = ControlledPawn->StartSuspicionPercent;
		break;
	case EDetectionState::Investigating:
		DetectionPercentage = ControlledPawn->StartInvestigatePercent;
		break;
	case EDetectionState::Alert:
		DetectionPercentage = 1;
		break;
	default:
		break;
	}
	SetDetectionState(DetectionState);
}

void ANPC_AIController::ToggleSuspicionDetection(const bool bEnabled)
{
	bSuspicionDetectionEnabled = bEnabled;
	if (!bEnabled)
	{
		ResetDetectionToState(EDetectionState::Neutral);
	}
}

void ANPC_AIController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	return;
	for (const AActor* Actor : UpdatedActors)
	{
		const TArray<FAIStimulus>& LastSensedSitmuli = GetPerceptionComponent()->GetActorInfo(*Actor)->
			LastSensedStimuli;
		for (const FAIStimulus& Stimulus : LastSensedSitmuli)
		{
		}
	}
}