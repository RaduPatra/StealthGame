// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_AnyFocus.h"

void UBTService_AnyFocus::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	FocusPriority = OverridenFocusPriority;
}
