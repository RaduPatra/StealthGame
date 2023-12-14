// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_AnyFocus.h"

void UBTService_AnyFocus::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FocusPriority = OverridenFocusPriority;
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}
