// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InputConfig.generated.h"

enum class ETriggerEvent : uint8;
/**
 * 
 */
class UInputAction;

USTRUCT()
struct FInputData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputAction;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETriggerEvent, FGameplayTag> EventToSendForTrigger;
};

UCLASS()
class STEALTHGAME_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UInputAction* GetInputActionForTag(const FGameplayTag& InputTag)
	{
		const FInputData* InputData = GetInputDataForTag(InputTag);
		if (!ensureAlways(InputData)) return nullptr;
		return InputData->InputAction;
	}

	FInputData* GetInputDataForTag(const FGameplayTag& InputTag)
	{
		FInputData* InputData = InputActions.Find(InputTag);
		return InputData;
	}

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Action.Trigger"))
	TMap<FGameplayTag, FInputData> AbilityInputActions;

private:
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Input"))
	TMap<FGameplayTag, FInputData> InputActions;
};
