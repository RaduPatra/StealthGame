// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIDefinitions.h"

#include "NPC_Character.generated.h"
class UAIPerceptionComponent;
class UAISettings;

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

	UPROPERTY(VisibleAnywhere)
	UAISettings* SettingsComponent;

	



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


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
