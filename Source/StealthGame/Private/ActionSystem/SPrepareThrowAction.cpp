// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SPrepareThrowAction.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"


bool USPrepareThrowAction::CanStart(FGameplayActionData& MyActionData, USGameplayActionComponent* OwnerComp) const
{
	return Super::CanStart(MyActionData, OwnerComp);
}

void USPrepareThrowAction::CancelAction()
{
	Super::CancelAction();
}

void USPrepareThrowAction::ActionActivated()
{
	ensureAlways(GetOwnerActor()->Implements<USThrower>());
	ThrowSplineComponent = ISThrower::Execute_GetThrowSplineComponent(GetOwnerActor());
	Super::ActionActivated();
}

void USPrepareThrowAction::ActionEnded()
{
	Super::ActionEnded();
}

void USPrepareThrowAction::CleanupSpline()
{
	ThrowSplineComponent->ClearSplinePoints();
	for (const auto SplineMesh : SplineMeshComponents)
	{
		SplineMesh->DestroyComponent(); //maybe poll this in the future?
	}
	SplineMeshComponents.Empty();
}

void USPrepareThrowAction::DrawSplineMesh()
{
	for (int i = 0; i < ThrowSplineComponent->GetNumberOfSplinePoints() - 2; ++i)
	{
		USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
		NewSplineMesh->SetStaticMesh(SplineMeshToUse);
		NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewSplineMesh->CanCharacterStepUpOn = ECB_No;
		NewSplineMesh->SetCastShadow(false);
		NewSplineMesh->SetBoundsScale(3.f);
		// NewSplineMesh->SetMobility(EComponentMobility::Movable);

		FVector StartLocation;
		FVector StartTangent;
		FVector EndLocation;
		FVector EndTangent;
		ThrowSplineComponent->GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent,
		                                                         ESplineCoordinateSpace::World);
		ThrowSplineComponent->GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent,
		                                                         ESplineCoordinateSpace::World);

		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::Z);
		NewSplineMesh->SetStartScale(FVector2D(SplineScale, SplineScale));
		NewSplineMesh->SetEndScale(FVector2D(SplineScale, SplineScale));
		NewSplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
		NewSplineMesh->RegisterComponent();
		SplineMeshComponents.Add(NewSplineMesh);
	}
}

void USPrepareThrowAction::DisplayPredictedPath()
{
	CleanupSpline();
	TArray<FVector> OutPathPositions;
	PredictPath(OutPathPositions);
	ThrowSplineComponent->SetSplinePoints(OutPathPositions, ESplineCoordinateSpace::World, true);
	DrawSplineMesh();
}

void USPrepareThrowAction::PredictPath(TArray<FVector>& OutPathPositions)
{
	FHitResult OutHit;
	FVector OutLastTraceDestination;

	FVector StartPos = ISThrower::Execute_GetThrowLocation(GetOwnerActor());
	FVector LaunchVelocity = ISThrower::Execute_GetThrowVelocity(GetOwnerActor());

	TArray ActorsToIgnore{GetOwnerActor()};

	UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(
		GetWorld(),
		OutHit,
		OutPathPositions,
		OutLastTraceDestination,
		StartPos,
		LaunchVelocity,
		true,
		ProjectileRadius,
		ECC_WorldDynamic,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		1,
		15.0f,
		5.0f);

	DrawDebugSphere(GetWorld(),
	                OutHit.ImpactPoint,	
	                12,
	                12,
	                FColor::Red,
	                false,
	                0,
	                0,
	                1);
}

void USPrepareThrowAction::Initialize(FGameplayActionData& MyActionData, USGameplayActionComponent* NewOwnerComponent)
{
	Super::Initialize(MyActionData, NewOwnerComponent);
	bShouldTick = true;
}
