// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "SGCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USGCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEALTHGAME_API ISGCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsArmed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FGameplayTag GetTeamTag() const;
};
