// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SOpenable.generated.h"


UENUM(BlueprintType)
enum class EOpenState : uint8
{
	Opened,
	Closed,
	Locked
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnOpenStateChanged, EOpenState, OpenState, AActor*, Instigator);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USOpenable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEALTHGAME_API ISOpenable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EOpenState GetOpenState();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetOpenMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RegisterStateChange(const FOnOpenStateChanged& OnOpenStateChanged);

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
