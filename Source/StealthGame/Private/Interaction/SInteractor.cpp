// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/SInteractor.h"
#include "ActionSystem/SGameplayActionComponent.h"
#include "Interaction/SInteractable.h"
#include "Interaction/SInteractorSelectionStrategy.h"
#include "Interaction/SProximitySelectionStrategy.h"
#include "UI/WorldUserWidget.h"


USInteractor::USInteractor()
{
	PrimaryComponentTick.bCanEverTick = true;
	SelectionStrategyClass = USProximitySelectionStrategy::StaticClass();
}

void USInteractor::FinishInteract(bool bWasCancelled)
{
	if (IsInteracting())
	{
		ISInteractable::Execute_StopInteract(CurrentInteractable.GetObject(), bWasCancelled);
		OnInteractFinished.Broadcast(bWasCancelled);
	}
	CurrentInteractable = nullptr;
}

void USInteractor::CancelInteract()
{
	if (IsInteracting())
	{
		CurrentPreviewData.GrantedInteractionAction->CancelAction();
	}
}

void USInteractor::BeginPlay()
{
	Super::BeginPlay();
	InteractionFilters = USInteractionFilters::MAKE(GetOwner());
	SelectionStrategy = USInteractorSelectionStrategy::MAKE(SelectionStrategyClass, this);
	ActionComponent = GetOwner()->FindComponentByClass<USGameplayActionComponent>();

	ensure(ActionComponent);
	ensure(InteractionFilters);
	ensure(SelectionStrategy);

	FTimerHandle SelectionStrategyInitTimer;
	GetOwner()->GetWorldTimerManager().SetTimer( //maybe move to selection strategy?
		SelectionStrategyInitTimer, this, &USInteractor::UpdateInteractable, CheckInteractionFrequency, true);
}

void USInteractor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!SelectionStrategy) return;
	SelectionStrategy->EndPlay();
	Super::EndPlay(EndPlayReason);
}

bool USInteractor::StartInteract()
{
	UpdateInteractable();
	if (!CanInteract() ||
		!ISInteractable::Execute_TryStartInteract(CurrentPreviewData.Interactable.GetObject(), this))
	{
		return false;
	}

	CurrentPreviewData.GrantedInteractionAction->OnActionFinished.AddDynamic(this, &USInteractor::FinishInteract);
	CurrentInteractable = CurrentPreviewData.Interactable;

	/*maybe I should add a GetActionData method to interactable interface so each one can pass its own properties
	instead of having implement multiple interfaces and call them in the action*/

	FGameplayActionData ActionData(CurrentPreviewData.InteractableOwner);
	const bool bActionStarted = CurrentPreviewData.GrantedInteractionAction->TryStartAction(
		ActionData, ActionComponent);
	ensureAlways(bActionStarted == true);

	//hide widget when currently interacting.
	if (ActiveInteractionWidget)
	{
		ActiveInteractionWidget->RemoveFromParent();
	}
	CurrentPreviewData.SetInteractable(nullptr);

	return true;
}

void USInteractor::SetInteractionFilter(const EFilterType FilterType) const
{
	SelectionStrategy->SetFilter(FilterType);
}

bool USInteractor::CanInteract() const
{
	if (!ensure(ActionComponent)) return false;

	if (ActionComponent->ActiveTags.HasAny(CanNotInteractTags)) return false;
	if (!CurrentPreviewData.Interactable) return false;
	if (IsInteracting()) return false;

	if (!CurrentPreviewData.GrantedInteractionAction) return false;
	if (!CurrentPreviewData.InteractableOwner) return false;

	FGameplayActionData ActionData(CurrentPreviewData.InteractableOwner);
	if (!CurrentPreviewData.GrantedInteractionAction->CanStart(ActionData, ActionComponent))
		return false;

	return true;
}

void USInteractor::HandleInteractableChange()
{
	if (CurrentPreviewData.Interactable)
	{
		ISInteractable::Execute_PreviewInteraction(CurrentPreviewData.Interactable.GetObject(), this);

		FGameplayActionData ActionData(CurrentPreviewData.InteractableOwner);
		CurrentPreviewData.GrantedInteractionAction = ActionComponent->GrantAction(
			CurrentPreviewData.GetInteractionData().ActionToGrant,
			ActionData, false);
	}

	if (PreviousPreviewData.Interactable)
	{
		ISInteractable::Execute_StopPreviewInteraction(PreviousPreviewData.Interactable.GetObject(), this);
		ActionComponent->RemoveAction(PreviousPreviewData.GrantedInteractionAction);
		PreviousPreviewData.GrantedInteractionAction = nullptr;
	}
}

void USInteractor::ShowInteractionWidget(const FName& InteractionText)
{
	if (ActiveInteractionWidget == nullptr && ensure(InteractionWidget))
	{
		const APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			ActiveInteractionWidget = CreateWidget<UWorldUserWidget>(
				PlayerController, InteractionWidget);
		}
	}
	if (ActiveInteractionWidget)
	{
		ActiveInteractionWidget->AttachedActor = CurrentPreviewData.InteractableOwner;
		if (!ActiveInteractionWidget->IsInViewport())
		{
			ActiveInteractionWidget->AddToViewport();
		}
	}
}

void USInteractor::UpdateInteractionText()
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PlayerController) return;

	if (CurrentPreviewData.Interactable)
	{
		//only update text if state has changed.
		if (CurrentPreviewData.InteractResult != CurrentPreviewData.PreviousInteractResult)
		{
			if (ActiveInteractionWidget == nullptr && ensure(InteractionWidget))
			{
				ActiveInteractionWidget = CreateWidget<UWorldUserWidget>(
					PlayerController, InteractionWidget);
			}

			if (ActiveInteractionWidget)
			{
				ActiveInteractionWidget->AttachedActor = CurrentPreviewData.InteractableOwner;
				if (!ActiveInteractionWidget->IsInViewport())
				{
					ActiveInteractionWidget->AddToViewport();
				}
			}

			OnInteractPreviewChanged.Broadcast(CurrentPreviewData.InteractResult, CurrentPreviewData.GetInteractionData());
		}
	}
	else
	{
		if (ActiveInteractionWidget)
		{
			ActiveInteractionWidget->RemoveFromParent();
		}
	}
}

void USInteractor::UpdateInteractable()
{
	if (IsInteracting()) return;

	PreviousPreviewData.SetInteractable(CurrentPreviewData.Interactable);
	CurrentPreviewData.SetInteractable(SelectionStrategy->SelectInteractableNew());

	if (PreviousPreviewData.Interactable != CurrentPreviewData.Interactable) //also check when abilitytogrant changes?
	{
		HandleInteractableChange();
		CurrentPreviewData.PreviousInteractResult = FInteractResult(false, TAG_ACTOR_INTERACTABLE_FAILREASON_INVALID);
	}

	//update interaction result
	if (!(CurrentPreviewData.Interactable && CanInteract()))
	{
		//todo - CanInteract() and ability fail reasons
		CurrentPreviewData.InteractResult = FInteractResult(false, TAG_ACTOR_INTERACTABLE_FAILREASON_CHARACTER);
	}
	else
	{
		const FInteractResult InteractableResult = ISInteractable::Execute_CanInteract(CurrentPreviewData.Interactable.GetObject(), this);
		CurrentPreviewData.InteractResult = InteractableResult;
	}

	UpdateInteractionText();
}
