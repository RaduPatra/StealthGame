// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CalculateLastKnownLocation.generated.h"

/**
 * 
 */

//rename to BTService_FindLastKnownLocation and stop using the old service
UCLASS()
class STEALTHGAME_API USBTService_CalculateLastKnownLocation : public UBTService 
{
	GENERATED_BODY()

public:
	USBTService_CalculateLastKnownLocation();

protected:
	/*UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LastSeenLocationKey;*/
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector IsInSightKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector IsInCombatKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AlertAwarenessOnKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector InvestigationAwarenessKey;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LastKnownLocationKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CurrentDetectionStateKey; //todo - add enum filter
	
	UPROPERTY(EditAnywhere)
	float NoEscapeRange = 500; //todo - add this to a config data asset or something and get it from controller instead of hardcoding it here

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActorKey;

	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	

private:
	void CalculateSenses(UBehaviorTreeComponent& OwnerComp) const;
	// virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

};
