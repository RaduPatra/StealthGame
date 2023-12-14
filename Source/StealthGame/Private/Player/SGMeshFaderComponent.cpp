// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SGMeshFaderComponent.h"

#include "Actor/SGFadeableMeshComponent.h"

// Sets default values for this component's properties
USGMeshFaderComponent::USGMeshFaderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USGMeshFaderComponent::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USGMeshFaderComponent::OccludeMeshes,
	                                       OcclusionCheckFrequency, true);
}

void USGMeshFaderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (USGFadeableMeshComponent* Comp : PreviousOccludingComponents)
	{
		Comp->RevertMesh();
	}
	Super::EndPlay(EndPlayReason);
}

void USGMeshFaderComponent::OccludeMeshes()
{
	//copilot get player camera manger from owner player controler:
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PlayerController) return;
	const APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
	if (!PlayerCameraManager) return;
	const FVector StartLocation = PlayerCameraManager->GetCameraLocation();
	
	FVector EyesLocation;
	FRotator EyesRotation;
	GetOwner()->GetActorEyesViewPoint(EyesLocation, EyesRotation);

	// DrawDebugSphere(GetWorld(), EndLocation, 22.f, 12, FColor::Red, true, .3);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	constexpr float Radius = 40.f;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();
	CameraRotation.Pitch = 0.f;
	const FVector EndLocation = EyesLocation - CameraRotation.Vector() * 50.f;

	GetWorld()->SweepMultiByObjectType(HitResults, StartLocation, EndLocation, FQuat::Identity, ObjectQueryParams,
	                                   FCollisionShape::MakeSphere(Radius), CollisionQueryParams);

	ProcessHitResults(HitResults);
}

void USGMeshFaderComponent::ProcessHitResults(const TArray<FHitResult>& HitResults)
{
	TArray<USGFadeableMeshComponent*> OccludingComponents;
	for (const FHitResult& HitResult : HitResults)
	{
		USGFadeableMeshComponent* FadeableComp = HitResult.GetActor()->FindComponentByClass<USGFadeableMeshComponent>();
		if (!FadeableComp) continue;
		OccludingComponents.AddUnique(FadeableComp);
	}

	const TArray<USGFadeableMeshComponent*> RemovedComponents = PreviousOccludingComponents.FilterByPredicate(
		[&](USGFadeableMeshComponent* MeshComponent)
		{
			return !OccludingComponents.Contains(MeshComponent);
		});

	const TArray<USGFadeableMeshComponent*> AddedComponents = OccludingComponents.FilterByPredicate(
		[&](USGFadeableMeshComponent* MeshComponent)
		{
			return !PreviousOccludingComponents.Contains(MeshComponent);
		});

	for (USGFadeableMeshComponent* Comp : AddedComponents)
	{
		Comp->OccludeMesh();
	}

	for (USGFadeableMeshComponent* Comp : RemovedComponents)
	{
		Comp->RevertMesh();
	}

	PreviousOccludingComponents = OccludingComponents;
}
