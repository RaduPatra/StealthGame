// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "SGSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class STEALTHGAME_API USGSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	void EnableCameraLag(float LagSpeed);
	void DisableCameraLag();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float CameraLagTarget;

	UPROPERTY(EditDefaultsOnly)
	float InterpSpeed;

private:
	bool bIsDisablingCameraLag;
	float CameraLagSpeedBeforeDisable;
};
