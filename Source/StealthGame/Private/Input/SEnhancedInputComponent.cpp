// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/SEnhancedInputComponent.h"

void USEnhancedInputComponent::UnbindActionEventByTag(const FGameplayTag& InputTag, ETriggerEvent TriggerEvent)
{
	const int* Handle = ActionEventHandles.Find({InputTag, TriggerEvent});
	if (!Handle) return;
	bool bRemoved = RemoveBindingByHandle(*Handle);
}

void USEnhancedInputComponent::UnbindAllActionEventsByTag(const FGameplayTag& InputTag)
{
	for (const auto Event : ActionEventHandles)
	{
		const auto TagForEvent = Event.Key;
		if (FGameplayTag EventTag = TagForEvent.Key; EventTag == InputTag)
		{
			RemoveBindingByHandle(Event.Value);
		}
	}
}

bool USEnhancedInputComponent::IsTagBoundToEvent(const FGameplayTag& InputTag, ETriggerEvent TriggerEvent)
{
	const TArray<TUniquePtr<FEnhancedInputActionEventBinding>>& Bindings = GetActionEventBindings();

	const int* Handle = ActionEventHandles.Find({InputTag, TriggerEvent});
	if (!Handle) return false;

	//if the handle is found, then it is bound to the event
	for (const auto& Elem : Bindings)
	{
		if (Elem->GetHandle() == *Handle)
		{
			return true;
		}
	}

	return false;
}
