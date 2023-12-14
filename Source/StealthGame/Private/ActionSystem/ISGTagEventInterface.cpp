// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/ISGTagEventInterface.h"

#include "ActionSystem/SGGameplayTagEventHandler.h"


// Add default functionality here for any IISGTagEventInterface functions that are not pure virtual.
void IISGTagEventInterface::SendGameplayEvent2_Implementation(const FGameplayTag EventTag)
{
	USGGameplayTagEventHandler* EventHandler = GetGameplayTagEventHandler();
	ensureAlways(EventHandler);
	EventHandler->SendGameplayEvent(EventTag);
}

void IISGTagEventInterface::RegisterGameplayTagEvent2_Implementation(FGameplayTag EventTag,
                                                                    const FSGOnGameplayTagEvent2& Delegate)
{
	USGGameplayTagEventHandler* EventHandler = GetGameplayTagEventHandler();
	ensureAlways(EventHandler);
	EventHandler->RegisterGameplayTagEvent(EventTag, Delegate);
}
