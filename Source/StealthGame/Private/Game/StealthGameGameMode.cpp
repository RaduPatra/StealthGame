// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/StealthGameGameMode.h"

#include "Game/ObjectPooling/SGObjectPoolingSubsystem.h"
#include "UObject/ConstructorHelpers.h"

AStealthGameGameMode::AStealthGameGameMode()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
	PrimaryActorTick.bCanEverTick = true;
}

void AStealthGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	USGObjectPoolingSubsystem* ObjectPoolingSubsystem = GetWorld()->GetSubsystem<USGObjectPoolingSubsystem>();
	if (ensure(ObjectPoolingSubsystem))
	{
		for (const FPooledTypeInfo& PooledTypeInfo : PooledTypes)
		{
			ObjectPoolingSubsystem->SetupPoolForType(PooledTypeInfo.PooledClass, PooledTypeInfo.PoolSize);
		}
	}
}

void AStealthGameGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void AStealthGameGameMode::RespawnPlayer(AController* Controller)
{
	if (ensure(Controller))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			Pawn->Destroy();
		}

		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}

void AStealthGameGameMode::HandlePlayerKilled(const APawn* Player, AActor* Killer)
{
	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;

	Delegate.BindUObject(this, &AStealthGameGameMode::RespawnPlayer, Player->GetController());

	GetWorldTimerManager().SetTimer(TimerHandle, Delegate, PlayerRespawnDelay, false);
}
