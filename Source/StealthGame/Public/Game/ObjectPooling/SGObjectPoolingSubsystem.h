// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/CircularQueue.h"
#include "Subsystems/WorldSubsystem.h"
#include "SGObjectPoolingSubsystem.generated.h"

/**
 * 
 */
typedef TCircularQueue<TWeakObjectPtr<AActor>> FActorPoolQueue;

USTRUCT()
struct FActorPool
{
	GENERATED_BODY()
	FActorPool() = default;
	FActorPool(int PoolSize)
	{
		FreeActors = MakeUnique<FActorPoolQueue>(PoolSize);
	}
	TUniquePtr<FActorPoolQueue> FreeActors;
};

template <>
struct TStructOpsTypeTraits<FActorPool> : public TStructOpsTypeTraitsBase2<FActorPool>
{
	enum { WithCopy = false };
};

UCLASS()
class STEALTHGAME_API USGObjectPoolingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	USGObjectPoolingSubsystem();
	void SetupPoolForType(TSubclassOf<AActor> Class, int PoolSize);

	UFUNCTION(BlueprintCallable)
	AActor* GetActorFromPool(TSubclassOf<AActor> ActorClass, const FTransform& Transform,
	                         AActor* Owner, APawn* Instigator);

	UFUNCTION(BlueprintCallable)
	bool ReleaseActorToPool(AActor* Actor);

	TMap<TSubclassOf<AActor>, FActorPool> ActorPoolMap;

};
