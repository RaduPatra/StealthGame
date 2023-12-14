// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SGameplayAction.h"
#include "SGActionTypes.h"
#include "UObject/Object.h"
#include "SGameplayActionComponent.generated.h"

/**
 * 
 */


// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionGranted, TSubclassOf<USGameplayAction>, ActionClass, AActor*,
//                                              Instigator);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnTagContainerChanged, const FGameplayTag, Tag, int32, Count);

UCLASS(BlueprintType)
class STEALTHGAME_API USGameplayActionComponent : public UActorComponent, public IISGTagEventInterface
{
	GENERATED_BODY()

public:
	USGameplayActionComponent();
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<USGameplayAction>> DefaultActions;

	UPROPERTY(BlueprintReadWrite)
	FSGGameplayTagCountContainer ActiveTags; //tags granted while an action is started

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer OtherTags; //tags manually added to the component (temp)
	
	UFUNCTION(BlueprintCallable)
	void RegisterContainerTagListener(FGameplayTag ActionTag, const FOnTagContainerChanged& Delegate);

	UFUNCTION(BlueprintCallable)
	USGameplayAction* GrantAction(TSubclassOf<USGameplayAction> ActionClass, UPARAM(ref) FGameplayActionData& NewActionData, bool bAutoStart);

	UFUNCTION(BlueprintCallable)
	void RemoveActionsWithClass(TSubclassOf<USGameplayAction> ActionClass);

	UFUNCTION(BlueprintCallable)
	USGameplayAction* TryStartActionWithTag(FGameplayTag Tag, UPARAM(ref) FGameplayActionData& NewActionData);

	UFUNCTION(BlueprintCallable)
	bool TryStopActionWithTag(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable)
	bool HasAction(TSubclassOf<USGameplayAction> Action);

	UFUNCTION(BlueprintCallable)
	bool HasActionFromInstigator(TSubclassOf<USGameplayAction> Action, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
	USGameplayAction* GetActionWithTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	void AddToActiveTags(const FGameplayTagContainer& Tags, float Count);
	
	TArray<USGameplayAction*> GetGrantedActions() const { return GrantedActions; }

	TArray<USGameplayAction*> GetActionsContainingTags(const FGameplayTagContainer& Tags) const;

	void RemoveAction(USGameplayAction* Action);

	virtual USGGameplayTagEventHandler* GetGameplayTagEventHandler() const override
	{
		return TagEventHandler;
	};
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

private:

	// UPROPERTY()
	// FGameplayActionData ActionData;
	
	UPROPERTY()
	TArray<USGameplayAction*> GrantedActions;

	TMap<FGameplayTag, FSGOnGameplayTagEvent> GameplayTagEventMap;

	UPROPERTY()
	USGGameplayTagEventHandler* TagEventHandler;
};
