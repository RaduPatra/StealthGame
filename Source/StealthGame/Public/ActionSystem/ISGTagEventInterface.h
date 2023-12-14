// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SGGameplayTagEventHandler.h"
#include "UObject/Interface.h"
#include "ISGTagEventInterface.generated.h"

class USGGameplayTagEventHandler;
// This class does not need to be modified.
UINTERFACE()
class UISGTagEventInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEALTHGAME_API IISGTagEventInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SendGameplayEvent2(const FGameplayTag EventTag);
	virtual void SendGameplayEvent2_Implementation(const FGameplayTag EventTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RegisterGameplayTagEvent2(FGameplayTag EventTag, const FSGOnGameplayTagEvent2& Delegate);
	virtual void RegisterGameplayTagEvent2_Implementation(FGameplayTag EventTag, const FSGOnGameplayTagEvent2& Delegate);

protected:
	virtual USGGameplayTagEventHandler* GetGameplayTagEventHandler() const = 0;
};
