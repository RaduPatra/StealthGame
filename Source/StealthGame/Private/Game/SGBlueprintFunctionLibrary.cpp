// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGBlueprintFunctionLibrary.h"

#include "ActionSystem/SGameplayActionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/MovementComponent.h"


// #include "DelayAction.h"

FGameplayTag USGBlueprintFunctionLibrary::RequestGameplayTag(FName TagName)
{
	return FGameplayTag::RequestGameplayTag(TagName);
}

UActorComponent* USGBlueprintFunctionLibrary::FindComponentByInterface(const AActor* InActor,
                                                                       const TSubclassOf<UInterface> Interface)
{
	if (TArray<UActorComponent*> FoundComponents = InActor->GetComponentsByInterface(Interface);
		FoundComponents.Num() > 0)
	{
		return FoundComponents[0];
	}
	return nullptr;
}

void USGBlueprintFunctionLibrary::TryBeginPlayForActor(AActor* Actor)
{
	if (!ensure(IsValid(Actor))) return;

	Actor->DispatchBeginPlay();
}

void USGBlueprintFunctionLibrary::ResetMovementComp(UMovementComponent* MoveComp)
{
	if (MoveComp->HasBeenInitialized()) return;

	MoveComp->Velocity = FVector(1, 0, 0);
	MoveComp->InitializeComponent();
}

bool USGBlueprintFunctionLibrary::HasAllTags_CountContainer(const FSGGameplayTagCountContainer& TagCountContainer,
                                                            const FGameplayTagContainer& OtherContainer,
                                                            bool bExactMatch)
{
	if (bExactMatch)
	{
		return TagCountContainer.HasAll(OtherContainer);
	}
	return TagCountContainer.HasAllExact(OtherContainer);
}

bool USGBlueprintFunctionLibrary::HasAnyTags_CountContainer(const FSGGameplayTagCountContainer& TagCountContainer,
                                                            const FGameplayTagContainer& OtherContainer,
                                                            bool bExactMatch)
{
	if (bExactMatch)
	{
		return TagCountContainer.HasAny(OtherContainer);
	}
	return TagCountContainer.HasAnyExact(OtherContainer);
}

void USGBlueprintFunctionLibrary::DelayForTicks(const UObject* WorldContextObject, int TicksToDelay,
                                                FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (LatentActionManager.FindExistingAction<class FDelayAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) ==
			NULL)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID,
			                                 new FDelayForTicksAction(TicksToDelay, LatentInfo));
		}
	}
}

void USGBlueprintFunctionLibrary::ForceGarbageCollection()
{
	GEngine->ForceGarbageCollection(true);
}

USGameplayAction* USGBlueprintFunctionLibrary::TryStartActionWithTag(AActor* ActionCompOwner,
                                                                     const FGameplayTag ActionTag,
                                                                     FGameplayActionData& ActionData)
{
	if (!ensure(ActionCompOwner)) return nullptr;

	USGameplayActionComponent* ActionComp = ActionCompOwner->FindComponentByClass<USGameplayActionComponent>();
	if (!ensure(ActionComp)) return nullptr;


	return ActionComp->TryStartActionWithTag(ActionTag, ActionData);
}

AActor* USGBlueprintFunctionLibrary::GetBlackboardValueAsActor(UBehaviorTreeComponent& OwnerComp,
                                                               FBlackboardKeySelector& KeySelector)
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp != nullptr)
	{
		return Cast<AActor>(BlackboardComp->GetValueAsObject(KeySelector.SelectedKeyName));
	}

	return nullptr;
}
