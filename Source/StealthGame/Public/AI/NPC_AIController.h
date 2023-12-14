// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIDefinitions.h"
// #include "AIManager.h"
#include "NPC_Character.h"
#include "AI/Tasks/SBTTask_TurnInPlace.h"
#include "Game/StealthGameGameMode.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPC_AIController.generated.h"

struct FAIStimulus;
class UAIManagerSubsystem;
/**
 * 
 */


UCLASS()
class STEALTHGAME_API ANPC_AIController : public AAIController
{
	GENERATED_BODY()
	ANPC_AIController();

public:
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void CreateSuspicionWidget();

	float CalculateSuspicionRate();

	UFUNCTION(BlueprintCallable)
	void ResetSuspicion();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EDetectionState CurrentDetectionState = EDetectionState::Neutral;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EAIState CurrentAIState = EAIState::Passive;

	UPROPERTY(BlueprintReadWrite) //move back to ctrl? 
	float DetectionPercentage;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDetectionStateChange(EDetectionState DetectionState);

	UFUNCTION(BlueprintCallable)
	void ResetDetectionToState(EDetectionState DetectionState);


	UFUNCTION(BlueprintCallable)
	void ToggleSuspicionDetection(bool bEnabled);

	UAIManagerSubsystem* GetAIManager() const
	{
		return GetWorld()->GetSubsystem<UAIManagerSubsystem>();
	};

protected:
	virtual void BeginPlay() override;

	//EditDefaultsOnly, 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UWorldUserWidget> SuspicionMeterWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	class ANPC_Character* ControlledPawn;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetAcquired(AActor* Target);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetLost(AActor* Target);

private:
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	virtual void PostInitializeComponents() override;

	EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard,
	                                                         FBlackboard::FKey ChangedKeyID);
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	virtual void PerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	void SetTargetActor(AActor* NewTarget);

	void CalculateDetectionLevel(float DeltaSeconds);

	void SetDetectionState(EDetectionState DetectionState);

	AActor* GetTargetActor() const;
	void HandleSightSense(AActor* Actor, const FAIStimulus& Stimulus);
	void HandleHearingSense(AActor* Actor, const FAIStimulus& Stimulus);

	bool CanTriggerSuspicion() const
	{
		return DetectionPercentage > ControlledPawn->StartSuspicionPercent &&
			DetectionPercentage < ControlledPawn->StartInvestigatePercent;
	}

	bool CanTriggerInvestigation() const
	{
		return DetectionPercentage > ControlledPawn->StartInvestigatePercent &&
			DetectionPercentage < 1;
	}

	bool CanTriggerAlert() const { return DetectionPercentage == 1; }

	bool CanDecreaseDetection() const
	{
		return DetectionPercentage < ControlledPawn->StartInvestigatePercent
			&& CurrentDetectionState != EDetectionState::Searching;
	}

	bool IsSuspicious() const { return CurrentDetectionState == EDetectionState::Searching; }

	bool IsInvestigating() const { return CurrentDetectionState == EDetectionState::Investigating; }

	bool IsAlert() const { return CurrentDetectionState == EDetectionState::Alert; }


	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer DetectedTeamTags;

	UPROPERTY(EditAnywhere)
	float EscapeRadius;

	bool bIsTargetInSight = false;

	UPROPERTY(EditDefaultsOnly)
	bool bCanSeeInstantlyDebug = false;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	TObjectPtr<UWorldUserWidget> ActiveSuspicionMeter;

	bool bSuspicionDetectionEnabled = true;
};
