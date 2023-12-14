// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SInteractorSelectionStrategy.h"
#include "SProximitySelectionStrategy.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class STEALTHGAME_API USProximitySelectionStrategy : public USInteractorSelectionStrategy
{
	GENERATED_BODY()

public:
	virtual TScriptInterface<ISInteractable> SelectInteractable() override;

	virtual void EndPlay() override;

protected:
	UFUNCTION()
	void OnInteractorBeginOverlap(AActor* MyActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorEndOverlap(AActor* MyActor, AActor* OtherActor);

	virtual void Init(USInteractor* Interactor) override;

	virtual void GatherInteractables(TArray<AActor*>& OutActors) override;

	UPROPERTY()
	TScriptInterface<ISInteractable> OverlapInteractable;
};
