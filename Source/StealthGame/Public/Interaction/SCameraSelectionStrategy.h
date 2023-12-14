// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractorSelectionStrategy.h"
#include "UObject/Object.h"
#include "SCameraSelectionStrategy.generated.h"

/**
 * 
 */
 class APawn;
 class APlayerController;
UCLASS(Blueprintable)
class STEALTHGAME_API USCameraSelectionStrategy : public USInteractorSelectionStrategy
{
	GENERATED_BODY()

protected:

	virtual TScriptInterface<ISInteractable> SelectInteractable() override;

	virtual void GatherInteractables(TArray<AActor*>& OutActors) override;
	
	virtual void Init(USInteractor* Interactor) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractRayLength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowDebug = false;

private:

	UPROPERTY()
	APawn* PlayerPawn;
	
	UPROPERTY()
	APlayerController* PlayerController;
	
};
