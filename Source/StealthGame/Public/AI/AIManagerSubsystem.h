// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "NPC_AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "UObject/NoExportTypes.h"
#include "AIManagerSubsystem.generated.h"
class ANPC_AIController;
/**
 * 
 */


USTRUCT()
struct FNoiseInfo
{
	GENERATED_BODY()

	FVector NoiseLocation;

	UPROPERTY()
	AActor* NoiseInstigator;
};

class UNavigationSystemV1;

UCLASS()
class STEALTHGAME_API UAIManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	void AssignNoiseStimuli();

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable)
	TArray<ANPC_AIController*> GetNPCControllers() const { return NPCControllers; }

	void SetTargetSense(AActor* Target, const FAIStimulus& Stimulus)
	{
		TargetStimuli.Add(Target, Stimulus);
	}

	void RegisterNPC(ANPC_AIController* Controller);
	void RegisterNoiseAwareNPC(ANPC_AIController* Controller);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	UPROPERTY()
	TArray<ANPC_AIController*> NPCControllers;

	UPROPERTY()
	TSet<ANPC_AIController*> NoiseAwareNPCs;

	UPROPERTY()
	TMap<AActor*, FAIStimulus> TargetStimuli;

	UPROPERTY()
	UNavigationSystemV1* NavSys = nullptr;
};
