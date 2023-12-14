// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SGDashToLocationAction.h"

#include "ActionSystem/SGDashAnimationData.h"

void USGDashToLocationAction::ActionActivated()
{
	const UDashActionData* DashActionData = Cast<UDashActionData>(ActionData.CustomActionParams);
	if (!ensure(DashActionData)) return;
	DashTargetLocation = DashActionData->DashTargetLocation;

	FVector DirToTarget = (DashTargetLocation - GetOwnerActor()->GetActorLocation()).GetSafeNormal();
	DirToTarget.Z = 0;
	FRotator RotToTarget = DirToTarget.Rotation();
	GetOwnerActor()->SetActorRelativeRotation(RotToTarget);

	const FVector StartLocation = GetOwnerActor()->GetActorLocation();

	float DashLoopDistance = FVector::Distance(StartLocation, DashTargetLocation)
		- DashAnimationData->DashStartDistance - DashAnimationData->DashEndDistance;

	if (DashLoopDistance < 0)
	{
		DashLoopDistance = 0;
	}

	DashLoopTime = DashLoopDistance / DashLoopSpeed;

	Super::ActionActivated();
}
