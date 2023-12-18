// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGMeshFaderComponent.generated.h"

class USGFadeableMeshComponent;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class STEALTHGAME_API USGMeshFaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USGMeshFaderComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OccludeMeshes();

public:
	UFUNCTION(BlueprintCallable)
	void ProcessHitResults(const TArray<FHitResult>& HitResults);

	UPROPERTY()
	TArray<USGFadeableMeshComponent*> PreviousOccludingComponents;

	UPROPERTY(EditDefaultsOnly)
	float OcclusionCheckFrequency = 0.1f;

private:
};
