// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SGSpringArmComponent.h"

void USGSpringArmComponent::EnableCameraLag(float LagSpeed)
{
	bIsDisablingCameraLag = false;
	bEnableCameraLag = true;
	CameraLagSpeed = LagSpeed;
}

void USGSpringArmComponent::DisableCameraLag()
{
	bIsDisablingCameraLag = true;
	CameraLagSpeedBeforeDisable = CameraLagSpeed;
}

void USGSpringArmComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDisablingCameraLag)
	{
		CameraLagSpeed = FMath::FInterpConstantTo(CameraLagSpeed, CameraLagTarget, DeltaTime, InterpSpeed);

		if (CameraLagSpeed == CameraLagTarget)
		{
			bIsDisablingCameraLag = false;
			bEnableCameraLag = false;
			CameraLagSpeed = CameraLagSpeedBeforeDisable;
		}
	}
}
