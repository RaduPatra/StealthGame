// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputConfig.h"
#include "SEnhancedInputComponent.generated.h"

/**
 * 
 */
class UInputConfig;


UCLASS()
class STEALTHGAME_API USEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template <class UserClass, typename FuncType>
	void BindActionByTag(UInputConfig* InputConfig, const FGameplayTag& InputTag,
	                     ETriggerEvent TriggerEvent,
	                     UserClass* Object, FuncType Func);

	template <class UserClass, typename FuncType1, typename FuncType2>
	void BindAbilityInputEvents(UInputConfig* InputConfig,
	                            UserClass* Object, FuncType1 TriggerFunc, FuncType2 CompletedFunc);

	void UnbindActionEventByTag(const FGameplayTag& InputTag, ETriggerEvent TriggerEvent);
	void UnbindAllActionEventsByTag(const FGameplayTag& InputTag);
	bool IsTagBoundToEvent(const FGameplayTag& InputTag, ETriggerEvent TriggerEvent);

private:
	TMap<TPair<FGameplayTag, ETriggerEvent>, int> ActionEventHandles; //rename to HandleForActionEvent
};

template <class UserClass, typename FuncType>
void USEnhancedInputComponent::BindActionByTag(
	UInputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	if (!ensureAlways(InputConfig)) return;
	if (UInputAction* InputAction = InputConfig->GetInputActionForTag(InputTag))
	{
		const FEnhancedInputActionEventBinding& Binding = BindAction(InputAction, TriggerEvent, Object, Func);
		int& Handle = ActionEventHandles.FindOrAdd({InputTag, TriggerEvent});
		Handle = Binding.GetHandle();
	}
}

template <class UserClass, typename FuncType1, typename FuncType2>
void USEnhancedInputComponent::BindAbilityInputEvents(UInputConfig* InputConfig,
                                                      UserClass* Object, FuncType1 TriggerFunc, FuncType2 CompletedFunc)
{
	if (!ensureAlways(InputConfig)) return;
	
	for (const auto Element : InputConfig->AbilityInputActions)
	{
		FGameplayTag AbilityTag = Element.Key;
		FInputData AbilityInputData = Element.Value;

		//bind triggered
		FGameplayTag TriggeredEventTag = AbilityInputData.EventToSendForTrigger.FindRef(ETriggerEvent::Triggered);

		const FEnhancedInputActionEventBinding& TriggeredBinding = BindAction(
			AbilityInputData.InputAction, ETriggerEvent::Triggered, Object, TriggerFunc, AbilityTag, TriggeredEventTag);

		int& TriggeredHandle = ActionEventHandles.FindOrAdd({AbilityTag, ETriggerEvent::Triggered});
		TriggeredHandle = TriggeredBinding.GetHandle();

		//bind completed
		FGameplayTag CompletedEventTag = AbilityInputData.EventToSendForTrigger.FindRef(ETriggerEvent::Completed);

		const FEnhancedInputActionEventBinding& CompletedBinding = BindAction(
			AbilityInputData.InputAction, ETriggerEvent::Completed, Object, CompletedFunc, AbilityTag,
			CompletedEventTag);

		int& CompletedHandle = ActionEventHandles.FindOrAdd({AbilityTag, ETriggerEvent::Completed});
		CompletedHandle = CompletedBinding.GetHandle();
	}
}
