// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SGameplayActionComponent.h"

#include "ActionSystem/SGGameplayTagEventHandler.h"

USGameplayActionComponent::USGameplayActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoRegister = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USGameplayActionComponent::BeginPlay()
{
	Super::BeginPlay();
	TagEventHandler = NewObject<USGGameplayTagEventHandler>(this);

	for (const auto Action : DefaultActions)
	{
		FGameplayActionData ActionData;
		ActionData.Instigator = GetOwner();
		GrantAction(Action, ActionData, false);
	}
}

void USGameplayActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (USGameplayAction* Action : GrantedActions)
	{
		if (Action->IsRunning() && Action->bShouldTick)
		{
			Action->TickAction(DeltaTime);
		}
	}
}


void USGameplayActionComponent::RegisterContainerTagListener(FGameplayTag ActionTag, const FOnTagContainerChanged& Callback)
{
	FSGOnGameplayEffectTagCountChanged& TagCountChangedDelegate = ActiveTags.RegisterGameplayTagEvent(
		ActionTag, EGameplayTagEventType::NewOrRemoved);
	TagCountChangedDelegate.AddLambda([&, Callback](const FGameplayTag Tag, int32 Count)
	{
		bool bExecuted = Callback.ExecuteIfBound(Tag, Count);
	});
}

USGameplayAction* USGameplayActionComponent::GrantAction(TSubclassOf<USGameplayAction> ActionClass,
                                                         FGameplayActionData& NewActionData, bool bAutoStart)
{
	if (ensure(ActionClass))
	{
		USGameplayAction* NewAction = USGameplayAction::CreateAction(ActionClass, NewActionData, this);

		if (bAutoStart)
		{
			bool bStarted = NewAction->TryStartAction(NewActionData, this);
		}

		GrantedActions.Add(NewAction);
		return NewAction;
	}

	return nullptr;
}

void USGameplayActionComponent::RemoveActionsWithClass(TSubclassOf<USGameplayAction> ActionClass)
{
	for (auto Action : GrantedActions)
	{
		if (Action->GetClass() == ActionClass)
		{
			GrantedActions.Remove(Action);
			ActiveTags.UpdateTagCount(Action->ActivationGrantedTags, -1);
		}
	}
}

TArray<USGameplayAction*> USGameplayActionComponent::GetActionsContainingTags(const FGameplayTagContainer& Tags) const
{
	TArray<USGameplayAction*> Actions;

	for (auto Action : GrantedActions)
	{
		if (Action->ActivationGrantedTags.HasAny(Tags))
		{
			Actions.Add(Action);
		}
	}

	return Actions;
}

void USGameplayActionComponent::RemoveAction(USGameplayAction* Action)
{
	const int NumRemoved = GrantedActions.Remove(Action);
	if (NumRemoved <= 0) return;
	ActiveTags.UpdateTagCount(Action->ActivationGrantedTags, -1);
}

USGameplayAction* USGameplayActionComponent::TryStartActionWithTag(FGameplayTag Tag, FGameplayActionData& NewActionData)
{
	for (USGameplayAction* const Action : GrantedActions)
	{
		if (Action->ActionStartTag == Tag)
		{
			if (Action->TryStartAction(NewActionData, this))
			{
				return Action;
			}
		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("Action Not Found or failed to start"));
	return nullptr;
}

USGameplayAction* USGameplayActionComponent::GetActionWithTag(const FGameplayTag& Tag)
{
	for (USGameplayAction* const Action : GrantedActions)
	{
		if (Action->ActionStartTag == Tag)
		{
			return Action;
		}
	}
	return nullptr;
}

void USGameplayActionComponent::AddToActiveTags(const FGameplayTagContainer& Tags, float Count)
{
	ActiveTags.UpdateTagCount(Tags, Count);
}


bool USGameplayActionComponent::TryStopActionWithTag(const FGameplayTag& Tag)
{
	for (USGameplayAction* const Action : GrantedActions)
	{
		if (Action->ActionStartTag == Tag)
		{
			// ActiveTags.RemoveTags(Action->GrantedTags);
			Action->FinishAction();
			return true;
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("Action Not Found"));
	return false;
}

bool USGameplayActionComponent::HasAction(TSubclassOf<USGameplayAction> Action)
{
	for (const USGameplayAction* GrantedAction : GrantedActions)
	{
		if (GrantedAction->IsA(Action))
		{
			return true;
		}
	}
	return false;
}

bool USGameplayActionComponent::HasActionFromInstigator(TSubclassOf<USGameplayAction> Action, AActor* Instigator)
{
	for (const USGameplayAction* GrantedAction : GrantedActions)
	{
		if (GrantedAction->IsA(Action) && GrantedAction->Instigator == Instigator)
		{
			return true;
		}
	}
	return false;
}
