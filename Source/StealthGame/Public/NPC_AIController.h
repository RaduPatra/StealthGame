// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIDefinitions.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPC_AIController.generated.h"

struct FAIStimulus;
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
	void CreateSuspicionWidget();
	float CalculateSuspiconRate();
	void CalculateDetectionPercentage(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	void ResetSuspicion();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EDetectionState CurrentDetectionState = EDetectionState::Neutral;

	UPROPERTY(BlueprintReadWrite) //move back to ctrl? 
	float DetectionPercentage;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnSuspicion();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDetectionStateChange(EDetectionState DetectionState);

	void SetDetectionState(EDetectionState DetectionState);
	

protected:
	virtual void BeginPlay() override;

private:
	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	virtual void PerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	void SetTargetActor(AActor* NewTarget);

	void CalculateDetectionLevel(float DeltaSeconds);

	AActor* GetTargetActor() const;

	bool CanTriggerSuspicion() const;

	bool CanTriggerInvestigation() const;

	bool IsSuspicious() const;

	bool IsInvestigating() const;

	bool IsAlert() const;

	bool CanTriggerAlert() const;

	bool CanDecreaseDetection() const;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere)
	float EscapeRadius;


	bool bIsTargetInSight = false;

	//switch to an enum later

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	class ANPC_Character* ControlledPawn;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UWorldUserWidget> SuspicionMeterWidgetClass;

	UPROPERTY()
	TObjectPtr<UWorldUserWidget> ActiveSuspicionMeter;
};
