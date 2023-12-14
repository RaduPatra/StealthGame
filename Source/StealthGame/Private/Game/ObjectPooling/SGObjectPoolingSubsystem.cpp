// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ObjectPooling/SGObjectPoolingSubsystem.h"
#include "Game/ObjectPooling/ISGPooledObject.h"

USGObjectPoolingSubsystem::USGObjectPoolingSubsystem()
{
}

void USGObjectPoolingSubsystem::SetupPoolForType(TSubclassOf<AActor> Class, int PoolSize)
{
	if (ActorPoolMap.Contains(Class)) return;
	ActorPoolMap.Emplace(Class, FActorPool(PoolSize));
	for (int i = 0; i < PoolSize; i++)
	{
		FActorSpawnParameters SpawnParameters;
		AActor* Actor = GetWorld()->SpawnActor(Class, &FTransform::Identity, SpawnParameters);
		ReleaseActorToPool(Actor);
	}
}

AActor* USGObjectPoolingSubsystem::GetActorFromPool(TSubclassOf<AActor> ActorClass, const FTransform& Transform,
                                                    AActor* Owner, APawn* Instigator)
{
	if (!ActorClass->GetDefaultObject<AActor>()->Implements<UISGPooledObject>()) return nullptr;

	TWeakObjectPtr<AActor> ActorPtr = nullptr;
	const FActorPool* Pool = ActorPoolMap.Find(ActorClass);
	if (!ensureAlways(Pool)) return nullptr;

	bool bEnqueueNewActor = false;
	
	if (Pool->FreeActors->Dequeue(ActorPtr))
	{
		//if I have a free actor in my pool, use it
		if (AActor* Actor = ActorPtr.Get())
		{
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);
			Actor->SetActorTickEnabled(true);
			Actor->SetActorTransform(Transform);
			Actor->SetOwner(Owner);
			Actor->SetInstigator(Instigator);

			Actor->DispatchBeginPlay();
			IISGPooledObject::Execute_PooledBeginPlay(Actor);
			
			return Actor;
		}

		//if the actor inside the pool was destroyed by someone else replace it with a new one
		bEnqueueNewActor = true;
	}

	//if I don't have a free actor in my pool spawn a new one
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Owner;
	SpawnParameters.Instigator = Instigator;
	ActorPtr = GetWorld()->SpawnActor(ActorClass, &Transform, SpawnParameters);

	IISGPooledObject::Execute_PooledBeginPlay(ActorPtr.Get());

	if (bEnqueueNewActor)
	{
		Pool->FreeActors->Enqueue(ActorPtr);
	}
	//todo - release actor back to pool when it's destroyed using OnDestroyed event. Need a secondary InUse map for this

	return ActorPtr.Get();
}

bool USGObjectPoolingSubsystem::ReleaseActorToPool(AActor* Actor)
{
	ensure(IsValid(Actor));
	if (!ensure(Actor->Implements<UISGPooledObject>())) return false;

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
	//todo - maybe UnregisterAllComponents?

	if (const FActorPool* Pool = ActorPoolMap.Find(Actor->GetClass()))
	{
		//if the pool is full, destroy the actor
		if (!Pool->FreeActors->Enqueue(Actor))
		{
			Actor->Destroy();
		}
	}

	IISGPooledObject::Execute_PooledEndPlay(Actor);
	Actor->RouteEndPlay(EEndPlayReason::Destroyed);
	return true;
	
}
