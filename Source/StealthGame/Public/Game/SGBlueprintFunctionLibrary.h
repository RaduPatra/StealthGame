// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LatentActions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "..\ActionSystem\SGActionTypes.h"

#include "SGBlueprintFunctionLibrary.generated.h"
class UMovementComponent;
struct FBlackboardKeySelector;
class USGameplayAction;
/**
 * 
 */
UCLASS()
class STEALTHGAME_API USGBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	static FGameplayTag RequestGameplayTag(FName TagName);

	UFUNCTION(BlueprintCallable, Category = "Helpers")
	static UActorComponent* FindComponentByInterface(const AActor* InActor, const TSubclassOf<UInterface> Interface);

	UFUNCTION(BlueprintCallable, Category = "Helpers")
	static void TryBeginPlayForActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Object Pooling")
	static void ResetMovementComp(UMovementComponent* MovementComponent);


	UFUNCTION(BlueprintPure, Category="GameplayTags")
	static bool HasAllTags_CountContainer(const FSGGameplayTagCountContainer& TagCountContainer,
	                                      const FGameplayTagContainer& OtherContainer, bool bExactMatch);

	UFUNCTION(BlueprintPure, Category="GameplayTags")
	static bool HasAnyTags_CountContainer(const FSGGameplayTagCountContainer& TagCountContainer,
	                                      const FGameplayTagContainer& OtherContainer, bool bExactMatch);

	UFUNCTION(BlueprintCallable, Category="Utilities|FlowControl",
		meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo", TicksToDelay="2", Keywords="sleep"))
	static void DelayForTicks(const UObject* WorldContextObject, int TicksToDelay, struct FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, Category="Debug")
	static void ForceGarbageCollection();

	static USGameplayAction* TryStartActionWithTag(AActor* ActionCompOwner, FGameplayTag ActionTag,
	                                               FGameplayActionData& ActionData);
	static AActor* GetBlackboardValueAsActor(class UBehaviorTreeComponent& OwnerComp,
	                                         FBlackboardKeySelector& KeySelector);
};


class FDelayForTicksAction : public FPendingLatentAction
{
public:
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	int TicksRemaining;

	FDelayForTicksAction(float TicksToDelay, const FLatentActionInfo& LatentInfo)
		: ExecutionFunction(LatentInfo.ExecutionFunction)
		  , OutputLink(LatentInfo.Linkage)
		  , CallbackTarget(LatentInfo.CallbackTarget)
		  , TicksRemaining(TicksToDelay)

	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		TicksRemaining--;
		Response.FinishAndTriggerIf(TicksRemaining <= 0, ExecutionFunction, OutputLink, CallbackTarget);
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		return NSLOCTEXT("DelayForTicksAction", "DelayForTicksAction", "Delay for X ticks").ToString();
	}
#endif
};
