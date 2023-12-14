// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TurnInPlace/TurnInPlaceComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTurnInPlace.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnInPlaceCompleted);
UCLASS()
class STEALTHGAME_API UAsyncTurnInPlace : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FTurnInPlaceCompleted OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FTurnInPlaceCompleted OnInterrupted;

	UPROPERTY()
	UTurnInPlaceComponent* TurnInPlaceComponent;

	FRotator TargetRotation;

public:

	
	//a node will be created for each static function inside UBlueprintAsyncActionBase with BlueprintInternalUseOnly
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTurnInPlace* AsyncTurnInPlace(UTurnInPlaceComponent* TurnInPlaceComp, FRotator TargetRotation);
//TODO - just pass in the component instead of the character
	virtual void Activate() override;

	UFUNCTION()
	void TurnInPlaceFinished(bool bInterrupted);
	
	void TurnInPlace();
	virtual void BeginDestroy() override;
};
