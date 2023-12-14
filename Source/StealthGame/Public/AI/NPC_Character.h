// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIDefinitions.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"

#include "NPC_Character.generated.h"
class UAIPerceptionComponent;
class UAISettings;
class USGHealthComponent;

UCLASS()
class STEALTHGAME_API ANPC_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnHealthDepleted();

	// UPROPERTY(VisibleAnywhere)
	// UAISettings* SettingsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTurnInPlaceComponent* TurnInPlaceComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USGameplayActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USGHealthComponent* HealthComponent;

public:

	//todo - add these to settings component
	UPROPERTY(EditAnywhere)
	float SuspicionDecreaseRate = .1;

	UPROPERTY(EditAnywhere)
	float StartSuspicionPercent = .2;

	UPROPERTY(EditAnywhere)
	float StartInvestigatePercent = .6;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DetectionRateCurve;
};
