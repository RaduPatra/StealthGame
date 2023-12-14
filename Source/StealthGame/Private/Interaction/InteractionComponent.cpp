// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionComponent.h"
#include "Interaction/SInteractor.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

FInteractResult UInteractionComponent::TryStartInteract_Implementation(USInteractor* InteractorInstigator)
{
	if (!Execute_CanInteract(this, InteractorInstigator)) return false;;
	OnInteract.Broadcast(InteractorInstigator);
	CurrentInteractor = InteractorInstigator;
	return true;
}

FInteractResult UInteractionComponent::CanInteract_Implementation(USInteractor* Interactor) const
{
	if (!OnCanInteract.IsBound()) return false;
	if (IsBeingInteractedWith())
	{
		return FInteractResult(false, TAG_ACTOR_INTERACTABLE_FAILREASON_OCCUPIED);
	}
	const FInteractResult Result = OnCanInteract.Execute(Interactor);
	return Result;

	// if (!OnCanInteract.Execute(Interactor)) return false;
}

void UInteractionComponent::PreviewInteraction_Implementation(USInteractor* Interactor)
{
}

void UInteractionComponent::StopPreviewInteraction_Implementation(USInteractor* Interactor)
{
}

void UInteractionComponent::StopInteract_Implementation(bool bCancelled)
{
	ISInteractable::StopInteract_Implementation(bCancelled);
	OnInteractFinished.Broadcast(CurrentInteractor, bCancelled);
	CurrentInteractor = nullptr;
}

AActor* UInteractionComponent::GetInteractableOwnerActor()
{
	return GetOwner();
}

const FInteractionData& UInteractionComponent::GetInteractionData()
{
	return InteractionData;
}

void UInteractionComponent::ChangeInteractionData(const FInteractionData& NewData)
{
	InteractionData.ChangeInteractionData(NewData.InteractionText, NewData.ActionToGrant);
	int k = 1;
}
