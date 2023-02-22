// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"

#include "BBKeys.h"
#include "NPC_Character.h"
#include "WorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"

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
	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is Null! Don't forget to assign it!")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ANPC_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CalculateDetectionLevel(DeltaSeconds);
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

float ANPC_AIController::CalculateSuspiconRate()
{
	auto SenseConfig = GetPerceptionComponent()->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>());

	auto SightSenseConfig = Cast<UAISenseConfig_Sight>(SenseConfig);

	if (!ensure(SightSenseConfig)) return 0;
	float SightRadius = SightSenseConfig->SightRadius;

	if (!ensure(TargetActor)) return 0;

	auto DistanceToTarget = FVector::Distance(GetPawn()->GetActorLocation(), TargetActor->GetActorLocation());
	auto NormalizedDistance = UKismetMathLibrary::NormalizeToRange(DistanceToTarget, 0, SightRadius);

	return ControlledPawn->DetectionRateCurve->GetFloatValue(NormalizedDistance);
}

void ANPC_AIController::CalculateDetectionLevel(float DeltaSeconds)
{
	if (!ensureMsgf(ControlledPawn->DetectionRateCurve, TEXT("You need to assign the detection curve!!"))) return;
	if (bIsTargetInSight)
	{
		CreateSuspicionWidget();

		const float SuspicionRate = CalculateSuspiconRate();
		DetectionPercentage += SuspicionRate * DeltaSeconds;
		DetectionPercentage = FMath::Clamp(DetectionPercentage, 0, 1);

		if (CanTriggerSuspicion() && !IsSuspicious())
		{
			UE_LOG(LogTemp, Warning, TEXT("Started Sus"));

			//play anim, show '?' UI etc.. - only show visual queues here
			OnSuspicion();

			SetDetectionState(EDetectionState::Suspicious);
		}
		else if (CanTriggerInvestigation() && !IsInvestigating())
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

			if (CanTriggerSuspicion() && IsInvestigating())
			{
				SetDetectionState(EDetectionState::Suspicious);
			}
			else if (DetectionPercentage < ControlledPawn->StartSuspicionPercent && IsSuspicious())
			{
				SetDetectionState(EDetectionState::Neutral);
			}
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


void ANPC_AIController::PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Warning, TEXT("PerceptionUpdated"));

	auto DetectedPawn = Cast<APawn>(Actor);
	if (!DetectedPawn || !DetectedPawn->IsPlayerControlled()) return;

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		bIsTargetInSight = Stimulus.WasSuccessfullySensed();

		// if (IsValid(TargetActor)) return;
		if (bIsTargetInSight)
		{
			TargetActor = Actor;
		}
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


bool ANPC_AIController::CanTriggerSuspicion() const
{
	return DetectionPercentage > ControlledPawn->StartSuspicionPercent &&
		DetectionPercentage < ControlledPawn->StartInvestigatePercent;
}

bool ANPC_AIController::CanTriggerInvestigation() const
{
	return DetectionPercentage > ControlledPawn->StartInvestigatePercent &&
		DetectionPercentage < 1;
}

bool ANPC_AIController::CanTriggerAlert() const
{
	return DetectionPercentage == 1;
}

bool ANPC_AIController::IsSuspicious() const
{
	return CurrentDetectionState == EDetectionState::Suspicious;
}

bool ANPC_AIController::IsInvestigating() const
{
	return CurrentDetectionState == EDetectionState::Investigating;
}

bool ANPC_AIController::IsAlert() const
{
	return CurrentDetectionState == EDetectionState::Alert;
}


bool ANPC_AIController::CanDecreaseDetection() const
{
	return DetectionPercentage < ControlledPawn->StartInvestigatePercent;
}

void ANPC_AIController::ResetSuspicion()
{
	DetectionPercentage = 0;

	SetDetectionState(EDetectionState::Neutral);
	GetBlackboardComponent()->SetValueAsEnum(BBKeys::DetectionStateKey, static_cast<uint8>(CurrentDetectionState));
}


void ANPC_AIController::SetDetectionState(EDetectionState DetectionState)
{
	CurrentDetectionState = DetectionState;
	GetBlackboardComponent()->SetValueAsEnum(BBKeys::DetectionStateKey, static_cast<uint8>(CurrentDetectionState));
	OnDetectionStateChange(CurrentDetectionState);
}
