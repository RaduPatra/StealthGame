// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SDashAction.h"
#include "ActionSystem/SGameplayAction.h"
#include "SGDashToLocationAction.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHGAME_API USGDashToLocationAction : public USDashAction
{
	GENERATED_BODY()

protected:
	virtual void ActionActivated() override;
	FVector DashTargetLocation;
};
