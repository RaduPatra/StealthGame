// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_Character.h"

#include "AISettings.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"

// Sets default values
ANPC_Character::ANPC_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SettingsComponent = CreateDefaultSubobject<UAISettings>(TEXT("AI Settings"));
}

// Called when the game starts or when spawned

void ANPC_Character::BeginPlay()
{
	Super::BeginPlay();
}



// Called every frame
void ANPC_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPC_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
