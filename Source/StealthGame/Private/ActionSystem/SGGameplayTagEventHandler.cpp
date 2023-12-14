// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SGGameplayTagEventHandler.h"

void USGGameplayTagEventHandler::SendGameplayEvent(const FGameplayTag EventTag)
{
	if (const FSGOnGameplayTagEvent2* EventToBroadcast = GameplayTagEventMap.Find(EventTag))
	{
		bool bExecuted = EventToBroadcast->ExecuteIfBound();
		int k = 1;
	}
}

void USGGameplayTagEventHandler::RegisterGameplayTagEvent(const FGameplayTag EventTag, const FSGOnGameplayTagEvent2& Delegate)
{
	GameplayTagEventMap.FindOrAdd(EventTag) = Delegate;
}
