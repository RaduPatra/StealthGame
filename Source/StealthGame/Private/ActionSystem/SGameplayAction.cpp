// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SGameplayAction.h"

#include "Player/StealthGameCharacter.h"
#include "ActionSystem/SGameplayActionComponent.h"
#include "ActionSystem/SGGameplayTagEventHandler.h"

USGameplayAction::USGameplayAction()
{

	// Make blueprint version true by default if not implemented so designers don't have to worry about base functionality
	auto ImplementedInBlueprint = [](const UFunction* Func) -> bool
	{
		return Func && ensure(Func->GetOuter())
			&& Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass());
	};

	{
		static FName FuncName = FName(TEXT("ReceiveCanStart"));
		UFunction* CanStartFunction = GetClass()->FindFunctionByName(FuncName);
		bHasBlueprintCanStart = ImplementedInBlueprint(CanStartFunction);
	}
}

USGameplayAction* USGameplayAction::CreateAction(TSubclassOf<USGameplayAction> ActionClass, FGameplayActionData& ActionData,
                                                 USGameplayActionComponent* OwnerComponent)
{
	USGameplayAction* NewAction = NewObject<USGameplayAction>(OwnerComponent->GetOwner(), ActionClass);
	NewAction->TagEventHandler = NewObject<	USGGameplayTagEventHandler>(NewAction);
	NewAction->Initialize(ActionData, OwnerComponent);
	return NewAction;
}

void USGameplayAction::Initialize(FGameplayActionData& NewActionData, USGameplayActionComponent* NewOwnerComponent)
{
	ActionData = NewActionData;
	Instigator = ActionData.Instigator;
	OwnerComponent = NewOwnerComponent;
	OwnerActor = OwnerComponent->GetOwner();
}


bool USGameplayAction::CanStart_Internal() const //todo  - remove
{
	if (OwnerComponent->ActiveTags.HasAny(ActivationBlockedTags))
	{
		return false;
	}

	if (ActivationRequiredTags.Num() > 0 && !OwnerComponent->ActiveTags.HasAny(ActivationRequiredTags))
	{
		return false;
	}
	return false;

}

bool USGameplayAction::TryStartAction(FGameplayActionData& NewActionData, USGameplayActionComponent* OwnerComp)
{
	if (!CanStart(NewActionData, OwnerComp))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Action cannot start!"))
		// CancelAction();
		return false;
	}

	TArray<USGameplayAction*> CancellableActions = OwnerComponent->GetActionsContainingTags(ActionCancelTags);
	for (const auto Action : CancellableActions)
	{
		if (Action->IsRunning())
		{
			Action->CancelAction();
		}
	}

	OwnerComponent->ActiveTags.UpdateTagCount(ActivationGrantedTags, 1);

	ActionData = NewActionData;
	Instigator = ActionData.Instigator;
	bStarted = true;
	ActionActivated();
	return true;
}

void USGameplayAction::FinishAction()
{
	if (!IsRunning()) return;

	OnActionFinished.Broadcast(false);
	ActionEnded();
}

void USGameplayAction::ActionActivated()
{
	OnActionStarted.Broadcast();
	ReceiveActionActivated();
}

void USGameplayAction::ActionEnded()
{
	bStarted = false;

	if (bShouldAutoRemoveOnFinish)
	{
		OwnerComponent->RemoveAction(this);
	}
	else
	{
		OwnerComponent->ActiveTags.UpdateTagCount(ActivationGrantedTags, -1);
	}
	ReceiveActionEnded();
}

void USGameplayAction::TickAction(float DeltaTime)
{
	ReceiveTickAction(DeltaTime);
}

bool USGameplayAction::CanStart(FGameplayActionData& MyActionData, USGameplayActionComponent* OwnerComp) const
{
	if (IsRunning())
	{
		return false;
	}
	
	if (OwnerComp->ActiveTags.HasAny(ActivationBlockedTags))
	{
		return false;
	}

	if (ActivationRequiredTags.Num() > 0 && !OwnerComp->ActiveTags.HasAny(ActivationRequiredTags))
	{
		return false;
	}
		
	if (bHasBlueprintCanStart)
	{
		return ReceiveCanStart(MyActionData, OwnerComp); 
	}
	
	return true;
}

bool USGameplayAction::IsRunning() const
{
	return bStarted;
}

UWorld* USGameplayAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T>
	UObject* Outer = GetOuter();
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}

	return nullptr;
}

void USGameplayAction::CancelAction() //cancel from outside 
{
	// if (!IsRunning()) return;

	OnActionFinished.Broadcast(true);
	ActionEnded();
}
