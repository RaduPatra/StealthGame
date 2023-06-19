// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckEscapeDistance.generated.h"

/**
 * Old class used for the previous last known location logic
 */
UCLASS()
class STEALTHGAME_API UBTService_CheckEscapeDistance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckEscapeDistance();
protected:
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector InEscapeRangeKey;

	UPROPERTY(EditAnywhere)
	float NoEscapeRange = 500;


	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
