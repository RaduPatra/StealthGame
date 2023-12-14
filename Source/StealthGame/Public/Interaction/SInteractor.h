// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SInteractable.h"
#include "Interaction/SInteractionTypes.h"
#include "Components/ActorComponent.h"

#include "SInteractor.generated.h"

class UInteractionComponent;
class ISInteractable;
class USInteractorSelectionStrategy;
class USInteractableStrategy;
class UWorldUserWidget;

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	Started,
	Finished,
	Interrupted
};

UENUM(BlueprintType)
enum class EPreviewInteractionState : uint8
{
	None,
	CanInteract,
	CanNotInteract
};

USTRUCT(BlueprintType)
struct FInteractionPreviewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ISInteractable> Interactable;

	UPROPERTY(BlueprintReadOnly)
	AActor* InteractableOwner;

	UPROPERTY(BlueprintReadOnly)
	FInteractionData InteractionData;

	UPROPERTY(BlueprintReadOnly)
	FInteractResult InteractResult = FInteractResult(false, TAG_ACTOR_INTERACTABLE_FAILREASON_INVALID);

	UPROPERTY(BlueprintReadOnly)
	FInteractResult PreviousInteractResult = FInteractResult(false, TAG_ACTOR_INTERACTABLE_FAILREASON_INVALID);;

	UPROPERTY()
	USGameplayAction* GrantedInteractionAction;

	void SetInteractable(const TScriptInterface<ISInteractable>& PreviewedInteractable)
	{
		Interactable = PreviewedInteractable;
		if (Interactable)
		{
			InteractableOwner = Interactable->GetInteractableOwnerActor();
		}
		else
		{
			InteractableOwner = nullptr;
		}
		PreviousInteractResult = InteractResult;
	}

	const FInteractionData& GetInteractionData() const
	{
		ensure(Interactable);
		return Interactable->GetInteractionData();
	}

	AActor* GetInteractableOwnerActor() const
	{
		ensure(Interactable);
		if (!Interactable)
		{
			return nullptr;
		}
		return Interactable->GetInteractableOwnerActor();
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractPreviewChanged, FInteractResult, Result,
                                             const FInteractionData&, InteractionData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFinished, bool, bWasCancelled);

// DECLARE_DYNAMIC_DELEGATE_OneParam(FInteractableFilterDelegate, TArray<UActorComponent>& /*Interactables*/, TArray<UActorComponent>& /*FilteredInteractables*/);

class USGameplayActionComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEALTHGAME_API USInteractor : public UActorComponent
{
	GENERATED_BODY()

public:
	USInteractor();

	UFUNCTION(BlueprintCallable)
	bool StartInteract(); //bound to BP key press event

	UFUNCTION(BlueprintCallable)
	void FinishInteract(bool bWasCancelled);

	UFUNCTION(BlueprintCallable)
	void CancelInteract();

	// UFUNCTION(BlueprintCallable)
	// FGameplayTag GetInteractorTag() const { return InteractorTag; }

	UFUNCTION(BlueprintCallable)
	void UpdateInteractable();

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnInteractPreviewChanged OnInteractPreviewChanged;

	FOnInteractionFinished OnInteractFinished;

	UPROPERTY(EditDefaultsOnly)
	EFilterType DefaultFilterType;

	UFUNCTION(BlueprintCallable)
	void SetInteractionFilter(const EFilterType FilterType) const;


	USInteractionFilters* GetInteractionFilters() const { return InteractionFilters; }
	const FInteractionPreviewData& GetCurrentPreviewData() const { return CurrentPreviewData; }

protected:
	UFUNCTION(BlueprintCallable)
	bool CanInteract() const; //bound to BP key press event
	void HandleInteractableChange();
	void ShowInteractionWidget(const FName& InteractionText);
	void UpdateInteractionText();


	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USInteractorSelectionStrategy> SelectionStrategyClass;

	UPROPERTY(BlueprintReadOnly)
	USInteractorSelectionStrategy* SelectionStrategy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer CanNotInteractTags;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Character"))
	// FGameplayTag InteractorTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWorldUserWidget> InteractionWidget;

	UPROPERTY()
	UWorldUserWidget* ActiveInteractionWidget;

	UPROPERTY(EditDefaultsOnly)
	float CheckInteractionFrequency = .2f;

	UPROPERTY(BlueprintReadOnly)
	FInteractionPreviewData CurrentPreviewData;

	UPROPERTY()
	FInteractionPreviewData PreviousPreviewData;

	UPROPERTY(BlueprintReadOnly)
	USInteractionFilters* InteractionFilters;

private:
	UPROPERTY()
	TScriptInterface<ISInteractable> CurrentInteractable;

	bool IsInteracting() const { return CurrentInteractable != nullptr; }
	UPROPERTY()
	USGameplayActionComponent* ActionComponent;
};
