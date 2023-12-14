// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SThrower.generated.h"


class USplineComponent;
// This class does not need to be modified.
UINTERFACE()
class USThrower : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */


class STEALTHGAME_API ISThrower
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetThrowLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetThrowVelocity() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	USplineComponent* GetThrowSplineComponent();
	// virtual USplineComponent* GetThrowSplineComponent_Implementation() { return nullptr; };

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
