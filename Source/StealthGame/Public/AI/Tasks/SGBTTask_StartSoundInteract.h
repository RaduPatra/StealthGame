// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SGBTTask_StartSoundInteract.generated.h"

class USInteractor;
/**
 * 
 */
UCLASS()
class STEALTHGAME_API USGBTTask_StartSoundInteract : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USGBTTask_StartSoundInteract();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector SoundKey;

	UPROPERTY(EditAnywhere)
	FGameplayTag InteractBehaviorInjectionTag;
	
	UPROPERTY()
	USInteractor* Interactor;

	UPROPERTY()
	UBehaviorTree* SubtreeToStart;
	


	

};

