// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SGBTDecorator_Cooldown.h"

 void USGBTDecorator_Cooldown::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{
	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	if (InitType == EBTMemoryInit::Initialize)
	{
		//hack so it is not on cooldown at the start
		if (!bStartCooledDown)
		{
			DecoratorMemory->LastUseTimestamp = -CoolDownTime; 
		}
		else
		{
			DecoratorMemory->LastUseTimestamp = DBL_MIN;
		}
	}

	DecoratorMemory->bRequestedRestart = false;
}