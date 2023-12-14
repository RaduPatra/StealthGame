// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI\AIManagerSubsystem.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"
#include "GameFramework/GameModeBase.h"
#include "StealthGameGameMode.generated.h"


USTRUCT()
struct FPooledTypeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Object Pooling")
	TSubclassOf<AActor> PooledClass;

	UPROPERTY(EditDefaultsOnly, Category = "Object Pooling")
	int PoolSize = 10;
};

UCLASS(minimalapi)
class AStealthGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStealthGameGameMode();
	virtual void BeginPlay() override;

	// UAIManager* GetAIManager() const { return AIManager; }

	void HandlePlayerKilled(const APawn* Player, AActor* Killer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	float PlayerRespawnDelay = 2.f;

protected:
	virtual void Tick(float DeltaSeconds) override;

	void RespawnPlayer(AController* Controller);


	UPROPERTY(EditDefaultsOnly, Category = "Object Pooling", meta = (MustImplement = "ISGPooledObject"))
	TArray<FPooledTypeInfo> PooledTypes;

private:
	// UPROPERTY()
	// UAIManager* AIManager;
};
