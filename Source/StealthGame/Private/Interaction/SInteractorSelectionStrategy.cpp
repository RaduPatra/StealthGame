// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SInteractorSelectionStrategy.h"
#include "Interaction/InteractionComponent.h"


USInteractorSelectionStrategy* USInteractorSelectionStrategy::MAKE(
	TSubclassOf<USInteractorSelectionStrategy> SelectionClass, USInteractor* Interactor)
{
	if (!ensure(SelectionClass)) return nullptr;

	USInteractorSelectionStrategy* SelectionStrategy = NewObject<USInteractorSelectionStrategy>(
		Interactor->GetOwner(), SelectionClass);
	SelectionStrategy->Init(Interactor);
	return SelectionStrategy;
}

TScriptInterface<ISInteractable> USInteractorSelectionStrategy::SelectInteractableNew()
{
	TArray<AActor*> FoundActors;
	GatherInteractables(FoundActors);

	TArray<TScriptInterface<ISInteractable>> Interactables;
	for (const AActor* FoundActor : FoundActors)
	{
		TArray<UActorComponent*> InteractableComponents = FoundActor->GetComponentsByInterface(
			USInteractable::StaticClass());
		for (const auto InteractableComp : InteractableComponents)
		{
			Interactables.Add(InteractableComp);
		}
	}

	const TScriptInterface<ISInteractable> Interactable = CurrentSelectionFilter->GetBestInteractable(
		Interactables, InteractorOwner);
	return Interactable;
}

void USInteractorSelectionStrategy::SetFilter(const EFilterType FilterType)
{
	CurrentSelectionFilter = InteractionFilters->GetFilterByType(FilterType);
}

void USInteractorSelectionStrategy::Init(USInteractor* Interactor)
{
	InteractionFilters = Interactor->GetInteractionFilters();
	SetFilter(Interactor->DefaultFilterType);
	InteractorOwner = Interactor;
}

TScriptInterface<ISInteractable> USTypeInteractionFilter::GetBestInteractable(
	const TArray<TScriptInterface<ISInteractable>>& InInteractables, USInteractor* Interactor)
{
	for (const auto Interactable : InInteractables)
	{
		if (FGameplayTag InteractableType = ISInteractable::Execute_GetInteractableType(Interactable.GetObject());
			InteractableType.MatchesTagExact(TagToFilter))
		{
			return Interactable;
		}
	}

	return nullptr;
}

TScriptInterface<ISInteractable> USDotInteractionFilter::GetBestInteractable(
	const TArray<TScriptInterface<ISInteractable>>& InInteractables, USInteractor* Interactor)
{
	float MaxDot = -1;
	TScriptInterface<ISInteractable> BestInteractable = nullptr;
	FVector BestCameraToInteractable = FVector::ZeroVector;
	//todo - cache player controller in an init function so its not called every tick
	const APawn* PlayerPawn = Cast<APawn>(Interactor->GetOwner());
	const APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController not found"));
		return nullptr;
	}

	const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	const FVector CameraLocation = CameraManager->GetCameraLocation();
	const FVector CameraLookDir = CameraManager->GetCameraRotation().Vector();

	for (const auto Interactable : InInteractables)
	{
		const FVector InteractableLocation = Interactable->GetInteractableOwnerActor()->GetActorLocation();

		const FVector CameraToInteractable = (InteractableLocation - CameraLocation).GetSafeNormal();
		const float Dot = FVector::DotProduct(CameraLookDir, CameraToInteractable);
		if (Dot > MaxDot)
		{
			MaxDot = Dot;
			BestInteractable = Interactable;
			BestCameraToInteractable = CameraToInteractable;
		}
	}

	/*DrawDebugLine(GetWorld(),
	              CameraLocation,
	              CameraLocation + CameraLookDir * 500,
	              FColor::Red,
	              false,
	              .2, 0, 5.f);

	DrawDebugLine(GetWorld(),
				  CameraLocation,
				  CameraLocation + BestCameraToInteractable * 500,
				  FColor::Blue,
				  false,
				  .2, 0, 5.f);*/
	
	return BestInteractable;
}

TScriptInterface<ISInteractable> USDefaultInteractionFilter::GetBestInteractable(
	const TArray<TScriptInterface<ISInteractable>>& InInteractables, USInteractor* Interactor)
{
	if (InInteractables.Num())
	{
		return InInteractables[0];
	}
	return nullptr;
}
