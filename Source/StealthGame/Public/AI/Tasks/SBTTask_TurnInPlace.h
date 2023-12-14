// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_TurnInPlace.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API USBTTask_TurnInPlace : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USBTTask_TurnInPlace();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	TOptional<FRotator> GetTurnRotation(UBlackboardComponent*, const FVector& AILocation) const;


	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TurnLocationKey;

	// FOnTurnInPlaceCompleted OnTurnInPlaceCompleted;

	UPROPERTY()
	UTurnInPlaceComponent* TurnInPlaceComponent;


	UFUNCTION()
	void TurnEnded(bool bInterrupted);
	UPROPERTY()
	UBehaviorTreeComponent* MyOwnerComp;
};
