// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SGGameplayTagEventHandler.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE(FSGOnGameplayTagEvent2);

UCLASS()
class STEALTHGAME_API USGGameplayTagEventHandler : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SendGameplayEvent(const FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable)
	void RegisterGameplayTagEvent(const FGameplayTag EventTag, const FSGOnGameplayTagEvent2& Delegate);

private:

	UPROPERTY()
	TMap<FGameplayTag, FSGOnGameplayTagEvent2> GameplayTagEventMap;

};
