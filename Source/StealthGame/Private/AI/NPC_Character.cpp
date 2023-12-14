// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC_Character.h"

#include "AI/AISettings.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "Character/SGHealthComponent.h"
#include "ActionSystem/SGameplayActionComponent.h"
#include "Camera/CameraComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISense_Hearing.h"

ANPC_Character::ANPC_Character()
{
	PrimaryActorTick.bCanEverTick = false;
	// SettingsComponent = CreateDefaultSubobject<UAISettings>(TEXT("AI Settings"));
	TurnInPlaceComponent = CreateDefaultSubobject<UTurnInPlaceComponent>(TEXT("Turn In Place Component"));
	ActionComponent = CreateDefaultSubobject<USGameplayActionComponent>("ActionComponent");
	HealthComponent = CreateDefaultSubobject<USGHealthComponent>("HealthComponent");
}

// Called when the game starts or when spawned

void ANPC_Character::BeginPlay()
{
	Super::BeginPlay();
	ensureMsgf(DetectionRateCurve, TEXT("You need to assign the detection curve!!"));
	// HealthComponent->OnHealthDepleted.AddDynamic(this, &ANPC_Character::OnHealthDepleted);
}

void ANPC_Character::OnHealthDepleted()
{
}
