// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISGPooledObject.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UISGPooledObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEALTHGAME_API IISGPooledObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pooling")
	void PooledBeginPlay();
	virtual void PooledBeginPlay_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object Pooling")
	void PooledEndPlay();
	virtual void PooledEndPlay_Implementation() {};
};
