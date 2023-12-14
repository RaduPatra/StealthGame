// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SProximitySelectionStrategy.h"
#include "Interaction/InteractionComponent.h"

//todo - move to static bp library


TScriptInterface<ISInteractable> USProximitySelectionStrategy::SelectInteractable()
{
	AActor* OwnerActor = InteractorOwner->GetOwner();

	TSet<AActor*> OverlappingActors;
	OwnerActor->GetOverlappingActors(OverlappingActors);

	for (const AActor* OverlappingActor : OverlappingActors)
	{
		if (UActorComponent* InteractableComponent = FindComponentByInterface(
			OverlappingActor, USInteractable::StaticClass()))
		{
			const TScriptInterface<ISInteractable> Interactable(InteractableComponent);
			return Interactable;
		}
	}

	return nullptr;
}

void USProximitySelectionStrategy::OnInteractorBeginOverlap(AActor* MyActor, AActor* OtherActor)
{
	UActorComponent* InteractableComponent = FindComponentByInterface(OtherActor, USInteractable::StaticClass());
	OverlapInteractable = InteractableComponent;
}

void USProximitySelectionStrategy::OnActorEndOverlap(AActor* MyActor, AActor* OtherActor)
{
	OverlapInteractable = nullptr;
}

void USProximitySelectionStrategy::Init(USInteractor* Interactor)
{
	Super::Init(Interactor);
	if (!ensure(Interactor)) return;
	Interactor->GetOwner()->OnActorBeginOverlap.AddDynamic(
		this, &USProximitySelectionStrategy::OnInteractorBeginOverlap);
	Interactor->GetOwner()->OnActorEndOverlap.AddDynamic(this, &USProximitySelectionStrategy::OnActorEndOverlap);
}

void USProximitySelectionStrategy::GatherInteractables(TArray<AActor*>& OutActors)
{
	const AActor* OwnerActor = InteractorOwner->GetOwner();
	OwnerActor->GetOverlappingActors(OutActors);
}

void USProximitySelectionStrategy::EndPlay()
{
	if (!ensure(InteractorOwner)) return;

	InteractorOwner->GetOwner()->OnActorBeginOverlap.RemoveDynamic(
		this, &USProximitySelectionStrategy::OnInteractorBeginOverlap);
	InteractorOwner->GetOwner()->OnActorEndOverlap.
	                 RemoveDynamic(this, &USProximitySelectionStrategy::OnActorEndOverlap);
}
