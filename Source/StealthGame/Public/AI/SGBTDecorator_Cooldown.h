// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Cooldown.h"
#include "SGBTDecorator_Cooldown.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API USGBTDecorator_Cooldown : public UBTDecorator_Cooldown
{
	GENERATED_BODY()

public:
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                              EBTMemoryInit::Type InitType) const override;

	UPROPERTY(EditAnywhere, Category = Cooldown)
	bool bStartCooledDown = false;
};
