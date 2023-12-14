// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/SCameraSelectionStrategy.h"

#include "Interaction/InteractionComponent.h"
#include "Interaction/SInteractable.h"

//todo - move to static bp library

TScriptInterface<ISInteractable> USCameraSelectionStrategy::SelectInteractable()
{
	if (!PlayerController) return nullptr;
	const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	FVector CameraLocation = CameraManager->GetCameraLocation();
	FRotator CameraRotation = CameraManager->GetCameraRotation();

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + CameraRotation.Vector() * InteractRayLength;

	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), TraceEnd, 10, 12, FColor::White, false, .2);
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, .2, 0, 5.f);
	}

	// ray trace by channel
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(PlayerPawn->GetOwner());
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return nullptr;

	UActorComponent* InteractableComp = FindComponentByInterface(HitActor, USInteractable::StaticClass());
	TScriptInterface<ISInteractable> Interactable(InteractableComp);

	if (!Interactable) return nullptr;
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), TraceEnd, 20, 12, FColor::Red, false, .2);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Interactable found"));
	}

	return Interactable;
}

void USCameraSelectionStrategy::GatherInteractables(TArray<AActor*>& OutActors)
{
	if (!PlayerController) return;
	const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	FVector CameraLocation = CameraManager->GetCameraLocation();
	FRotator CameraRotation = CameraManager->GetCameraRotation();

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + CameraRotation.Vector() * InteractRayLength;

	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), TraceEnd, 10, 12, FColor::White, false, .2);
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, .2, 0, 5.f);
	}

	// ray trace by channel
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(PlayerPawn->GetOwner());
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return;

	/*UActorComponent* InteractableComp = FindComponentByInterface(HitActor, USInteractable::StaticClass());
	TScriptInterface<ISInteractable> Interactable(InteractableComp);

	if (!Interactable) return;
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), TraceEnd, 20, 12, FColor::Red, false, .2);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Interactable found"));
	}
	*/

	OutActors.Add(HitActor);
}


void USCameraSelectionStrategy::Init(USInteractor* Interactor)
{
	Super::Init(Interactor);
	if (!ensure(Interactor)) return;

	PlayerPawn = Cast<APawn>(Interactor->GetOwner());
	PlayerController = Cast<APlayerController>(PlayerPawn->GetController());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController not found"));
	}
}
