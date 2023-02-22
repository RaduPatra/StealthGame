// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AISettings.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEALTHGAME_API UAISettings : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAISettings();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	TArray<AActor*> GetPatrolPoints() const { return PatrolPoints; }


private:
	UPROPERTY(EditAnywhere)
	TArray<AActor*> PatrolPoints;
};
