// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SInteractable.h"
#include "Interaction/SInteractor.h"


void FInteractionData::ChangeInteractionData(const FName NewInteractionText,
                                             const TSubclassOf<USGameplayAction> NewActionToGrant)
{
	InteractionText = NewInteractionText;
	ActionToGrant = NewActionToGrant;
}

// Add default functionality here for any ISInteractable functions that are not pure virtual.
FInteractResult ISInteractable::TryStartInteract_Implementation(USInteractor* Interactor)
{
	// return CanInteract(Interactor);
	if (!CanInteract(Interactor)) return false;
	const APawn* InteractorPawn = Cast<APawn>(Interactor->GetOwner());
	if (InteractorPawn->GetController()->IsPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Interacted with interactable"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Interacted with interactable"));
	}
	return true;
}
