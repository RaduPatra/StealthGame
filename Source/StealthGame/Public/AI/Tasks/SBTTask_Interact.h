// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Interaction/SInteractor.h"
#include "SBTTask_Interact.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API USBTTask_Interact : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USBTTask_Interact();

protected:
	UFUNCTION()
	void InteractionFinished(bool bWasCancelled);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	UPROPERTY()
	USInteractor* InteractionComponent;
};
