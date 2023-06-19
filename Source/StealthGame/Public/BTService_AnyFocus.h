// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/Services/BTService_DefaultFocus.h"
#include "BTService_AnyFocus.generated.h"

/**
 * Focus service that allows us to override the default focus priority
 * We use it to change how AI rotates during movement
 */
UCLASS()
class STEALTHGAME_API UBTService_AnyFocus : public UBTService_DefaultFocus
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	uint8 OverridenFocusPriority = 0;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
