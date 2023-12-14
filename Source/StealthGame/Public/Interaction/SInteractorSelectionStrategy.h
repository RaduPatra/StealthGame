// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractor.h"
#include "Interaction/SInteractionTypes.h"
#include "SInteractorSelectionStrategy.generated.h"

/**c
 * 
 */
//todo - move to static lib
inline UActorComponent* FindComponentByInterface(const AActor* InActor, const TSubclassOf<UInterface> Interface)
{
	if (TArray<UActorComponent*> FoundComponents = InActor->GetComponentsByInterface(Interface);
		FoundComponents.Num() > 0)
	{
		return FoundComponents[0];
	}
	return nullptr;
}


class ISInteractable;
class USInteractor;

UCLASS(Abstract, BlueprintType)
class STEALTHGAME_API USInteractorSelectionStrategy : public UObject // could rename to USInteractionSelector?
{
	GENERATED_BODY()

public:
	static USInteractorSelectionStrategy* MAKE(TSubclassOf<USInteractorSelectionStrategy> SelectionClass,
	                                           USInteractor* Interactor);

	virtual TScriptInterface<ISInteractable> SelectInteractable() { return nullptr; }

	TScriptInterface<ISInteractable> SelectInteractableNew();


	virtual void GatherInteractables(TArray<AActor*>& OutActors) PURE_VIRTUAL(USInteractorSelectionStrategy::GatherInteractables, ;);

	void SetFilter(EFilterType FilterType);

	virtual void EndPlay()
	{
	}

protected:
	virtual void Init(USInteractor* Interactor);

	UPROPERTY()
	USInteractor* InteractorOwner;

	UPROPERTY()
	USInteractionSelectionFilter* CurrentSelectionFilter;

	UPROPERTY()
	USInteractionFilters* InteractionFilters;

	// virtual TArray<TScriptInterface<ISInteractable>>& GatherInteractables();
};


UCLASS(Abstract, BlueprintType)
class STEALTHGAME_API USInteractionSelectionFilter : public UObject
{
	GENERATED_BODY()

public:
	virtual TScriptInterface<ISInteractable>
	GetBestInteractable(const TArray<TScriptInterface<ISInteractable>>& InActors, USInteractor* Interactor) PURE_VIRTUAL(
		USInteractionSelectionFilter::GetBestInteractableForFoundActors, return nullptr;);
};


UCLASS(BlueprintType, DefaultToInstanced)
class STEALTHGAME_API USTypeInteractionFilter : public USInteractionSelectionFilter
{
	GENERATED_BODY()

	FGameplayTag TagToFilter;

public:
	UFUNCTION(BlueprintCallable)
	void SetFilterTag(const FGameplayTag Tag) { TagToFilter = Tag; }

	virtual TScriptInterface<ISInteractable> GetBestInteractable(
		const TArray<TScriptInterface<ISInteractable>>& InActors, USInteractor* Interactor) override;
};

UCLASS(BlueprintType, DefaultToInstanced)
class STEALTHGAME_API USDotInteractionFilter : public USInteractionSelectionFilter
{
	GENERATED_BODY()

public:
	virtual TScriptInterface<ISInteractable> GetBestInteractable(
		const TArray<TScriptInterface<ISInteractable>>& InActors, USInteractor* Interactor) override;
};

UCLASS(BlueprintType, DefaultToInstanced)
class STEALTHGAME_API USDefaultInteractionFilter : public USInteractionSelectionFilter
{
	GENERATED_BODY()

public:
	virtual TScriptInterface<ISInteractable> GetBestInteractable(
		const TArray<TScriptInterface<ISInteractable>>& InActors, USInteractor* Interactor) override;
};
