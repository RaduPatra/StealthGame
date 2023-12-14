// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TurnInPlace/AsyncTurnInPlace.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"

UAsyncTurnInPlace* UAsyncTurnInPlace::AsyncTurnInPlace(UTurnInPlaceComponent* TurnInPlaceComp, FRotator TargetRotation)
{
	UAsyncTurnInPlace* Node = NewObject<UAsyncTurnInPlace>();
	Node->TurnInPlaceComponent = TurnInPlaceComp;
	Node->TargetRotation = TargetRotation;
	return Node;
}

void UAsyncTurnInPlace::Activate()
{
	TurnInPlace();
	RegisterWithGameInstance(TurnInPlaceComponent->GetOwner());
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Activate"));
}

void UAsyncTurnInPlace::TurnInPlaceFinished(bool bInterrupted)
{
	if (bInterrupted)
	{
		OnInterrupted.Broadcast();
	}
	else
	{
		OnCompleted.Broadcast();
	}
	
	TurnInPlaceComponent->OnTurnInPlaceFinished.RemoveDynamic(this, &UAsyncTurnInPlace::TurnInPlaceFinished);
	SetReadyToDestroy();
}

void UAsyncTurnInPlace::TurnInPlace()
{
	if (!TurnInPlaceComponent) return;
	TurnInPlaceComponent->StartTurnInPlace(TargetRotation);
	TurnInPlaceComponent->OnTurnInPlaceFinished.AddDynamic(this, &UAsyncTurnInPlace::TurnInPlaceFinished);
}

void UAsyncTurnInPlace::BeginDestroy()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Async BeginDestroy"));

	if (TurnInPlaceComponent)
	{
		TurnInPlaceComponent->OnTurnInPlaceFinished.RemoveDynamic(this, &UAsyncTurnInPlace::TurnInPlaceFinished);
	}

	Super::BeginDestroy();
}
